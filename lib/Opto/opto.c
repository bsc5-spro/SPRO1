#include "opto.h"
#include <avr/io.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// arc distance between two gaps in [m]illi[m]eters
// assuming no slippage, the distance a point on the wheel travels equals the
// distance the car travels forwards
//
// The gear ratio is irrelevant as the optocoupler and encoder wheel are on the
// same shaft, meaning that they have the same angular velocity.
//
// ds = theta*(pi/180)*r
// theta = 20
// r = 20mm (radius of wheel)
#define DELTA_S_MM 0.00174532925
#define MAX_VEL_WIN_SIZE 20
#define MIN_VALID_TICKS 100
#define TIMING_CONSTANT 15625UL

static uint16_t previous_edge, current_edge, overflow_count;
static uint16_t delta_tick;

static uint16_t previous_tick;

static uint8_t vel_index = 0;
static uint8_t vel_count;
static uint16_t vel_sum = 0;
static uint16_t velocities[MAX_VEL_WIN_SIZE]; // array of measured velocities

static uint16_t total_distance = 0; // in mm, max ~65m
static uint16_t total_hundred_ticks = 0;
static uint16_t total_time = 0;

static unsigned char recording;

static void add_to_moving_average(uint16_t new_val);

void opto_init(void) {
  TCCR1A = 0x00; // pure ticks counter
  TCCR1B = 0xC5;

  previous_edge = ICR1;
  previous_tick = TCNT1;

  delta_tick = 0;

  recording = 0;
  vel_index = 0;
  vel_count = 0;
  zero_distance();
}

void monitor_encoder(void) {
  if (recording) {
    uint16_t temp = TCNT1;
    uint16_t dt = temp - previous_tick;
    previous_tick = temp;

    // somehow calcualte the time passed and add it to the running total
    uint16_t vel = get_current_velocity();
    add_to_moving_average(vel);
  }
}

uint16_t get_delta_ticks(void) {

  if (ICR1 != previous_edge) { // There has been a new
    // obstruction
    current_edge = ICR1;
    delta_tick = current_edge - previous_edge;

    if (delta_tick > MIN_VALID_TICKS) {
      previous_edge = current_edge;
      // block_count++;
      total_distance += DELTA_S_MM;
      return delta_tick;
    }
  }
  return 0;
}

void zero_distance() { total_distance = 0; }

void zero_time() {
  total_time = 0;
  total_hundred_ticks = 0;
}

// mm; max ~65m; *10^4
uint16_t get_distance_travelled(void) { return total_distance; }

uint16_t get_total_time(void) {}

unsigned char toggle_recording(void) {
  recording = !recording;
  return recording;
}

uint16_t get_current_velocity(void) {
  uint16_t ticks = get_delta_ticks();

  if (!ticks || ticks < MIN_VALID_TICKS)
    return 0;

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

  // replace odest value with new value
  velocities[vel_index] = new_val;
  vel_sum += new_val;

  // make sure that vel_index is less than MAX_VEL_WIN_SIZE
  vel_index = (vel_index + 1) % MAX_VEL_WIN_SIZE;
}
