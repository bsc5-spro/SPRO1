#include "opto.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

// arc distance between two gaps in [m]illi[m]eters
// assuming no slippage, the distance a point on the wheel travels equals the
// distance the car travels forwards
//
// The gear ratio is irrelevant as the optocoupler and encoder wheel are on the
// same shaft, meaning that they have the same angular velocity.
//
// ds = theta*(pi/180)*r
// theta = 18
// r = 31.3mm (radius of wheel)
#define DELTA_S_MM 19.5721
#define MAX_VEL_WIN_SIZE 10
#define MIN_VALID_TICKS 100
#define TIMING_CONSTANT 15625UL

static uint16_t previous_edge, current_edge; //, overflow_count;
static uint16_t delta_tick;
static uint16_t block_count;

static uint16_t previous_tick = 0;

static uint8_t vel_index = 0;
static uint8_t vel_count;
static uint16_t vel_sum = 0;
static uint16_t velocities[MAX_VEL_WIN_SIZE]; // array of measured velocities

static uint16_t total_distance = 0; // in mm, max ~65m
static uint16_t total_hundred_ticks = 0;
static uint16_t total_time = 0;

static unsigned char recording;

volatile uint16_t last_capture = 0;
volatile uint32_t delta_accumulator = 0;
volatile uint8_t delta_count = 0;

static void add_to_moving_average(uint16_t new_val);

void opto_init(void) {
  TCCR1A = 0x00; // pure ticks counter
  TCCR1B = 0xC5;
  TIMSK1 = (1 << ICIE1);

  sei();

  DDRB &= ~0x01; // PB0 as input
  PORTB |= 0x01; // pullup on PB0

  // previous_edge = ICR1;

  delta_tick = 0;
  block_count = 0;

  recording = 0;
  vel_index = 0;
  vel_count = 0;
  zero_distance();
}

/* TIMING
 *
 * MCU runs at 16MHz => 16 million ticks a second
 * TCCR1 runs with a 1024 prescaler -> 15625 ticks a sec
 *
 * Every tick corresponds to 0.000 064 seconds
 * or 4/625 hundreths of a second
 *
 * every 625 ticks, 4 hundreth of a second pass
 */

static char vel_invalid = 0;

static uint16_t sub_ticks = 0;

void monitor_encoder(void) {
  if (recording) {
    uint16_t now = TCNT1;
    uint16_t elapsed = now - previous_tick;
    previous_tick = now;

    sub_ticks += elapsed;
    while (sub_ticks >= 625) {
      sub_ticks -= 625;
      total_time += 4;
    }
  }

  if (delta_count >= 4) {
    uint16_t vel = get_current_velocity();
    if (vel == 0 && !vel_invalid) { // only add 0 velocity if previous was not
                                    // zero
      vel_invalid = 1;
    } else {
      add_to_moving_average(vel);
      vel_invalid = 0;
    }
  }
}

uint16_t get_delta_ticks(void) {
  uint16_t avg = 0;

  cli();
  if (delta_count > 0) {
    avg = delta_accumulator;
    delta_accumulator = 0;
    total_distance += DELTA_S_MM * delta_count;
    delta_count = 0;
  }
  sei();

  return avg;
}

void zero_distance() { total_distance = 0; }

void zero_time() {
  total_time = 0;
  total_hundred_ticks = 0;

  previous_tick = 0;
}

// mm; max ~65m; *10^4
uint16_t get_distance_travelled(void) { return total_distance; }

// hundreth of a sec; max 11 minutes
uint16_t get_total_time(void) { return total_time; }

unsigned char toggle_recording(void) {
  recording = !recording;
  return recording;
}

unsigned char start_recording(void) {
  recording = 1;
  return recording;
}

unsigned char stop_recording(void) {
  recording = 0;
  return recording;
}

// mm / s
uint16_t get_current_velocity(void) {
  uint16_t ticks = get_delta_ticks();

  if (!ticks || ticks < MIN_VALID_TICKS)
    return 0;

  // total_distance += DELTA_S_MM * block_count;
  block_count = 0;

  return (uint16_t)((DELTA_S_MM * TIMING_CONSTANT) / ticks);
}

uint16_t get_average_velocity(void) { return (uint16_t)(vel_sum / vel_count); }

void add_to_moving_average(uint16_t new_val) {
  // if the velocity array is full, remove the oldest value from the sum
  if (vel_count == MAX_VEL_WIN_SIZE) {
    vel_sum -= velocities[vel_index];
  } else {
    vel_count++;
  }

  // replace oldest value with new value
  velocities[vel_index] = new_val;
  vel_sum += new_val;

  // make sure that vel_index is less than MAX_VEL_WIN_SIZE
  vel_index = (vel_index + 1) % MAX_VEL_WIN_SIZE;
}

ISR(TIMER1_CAPT_vect) {
  uint16_t current = ICR1;
  uint16_t delta = current - last_capture;

  if (delta == 0) {
    return;
  }

  last_capture = current;

  delta_accumulator += delta;
  delta_count++;
}
