#include "opto.h"
#include <avr/io.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DELTA_S                                                                \
  0.00174532925 // arc distance between two gaps in [m]eters
                // ds = theta*(pi/180)*r
                // theta = 20
                // r = 5mm

#define VEL_WINDOW_SIZE 20

static uint16_t previous_edge, current_edge; //, overflow_count;
static unsigned long delta_tick;
static unsigned long block_count; // number of obstructions detected

static uint16_t previous_tick;

static int vel_index = 0;
static int vel_count;
static float vel_sum = 0;
static float velocities[VEL_WINDOW_SIZE]; // array of measured velocities

static float delta_s;

static char recording;

static char monitor;

static float get_current_velocity(void);
static void add_to_moving_average(float new_val);

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

  monitor_encoder();
}

void monitor_encoder(void) {
  if (recording) {
    float vel = get_current_velocity();
    add_to_moving_average(vel);
  }
}

unsigned long get_delta_ticks(void) {

  if (ICR1 != previous_edge) { // There has been a new
    // obstruction
    current_edge = ICR1;
    block_count++;

    delta_tick = current_edge - previous_edge;

    previous_edge = current_edge;

    return delta_tick;
  }
  return 0;
}

float get_delta_time(void) { // in ms
  if (ICR1 != previous_edge) {
    return (float)(get_delta_ticks() - 1) / 16;
  }
  return 0;
}

void zero_distance() { block_count = 0; }

float get_distance_travelled(void) { return (float)block_count * DELTA_S; }

unsigned char toggle_recording(void) {
  recording = (recording + 1) % 2;
  return recording;
}

float get_current_velocity(void) {
  unsigned char count = 0;
  float t = 0;
  do {
    count++;
    t = get_delta_time();
  } while (t == 0 || count < 5); // so there won't be any division with zero
  t = t / 1000;       // convert ms to s
  return DELTA_S / t; // therefore output is m/s
}

float get_average_velocity(int n) { return vel_sum / vel_count; }

void add_to_moving_average(float new_val) {
  if (vel_count == VEL_WINDOW_SIZE) {
    vel_sum -= velocities[vel_index];
  } else {
    vel_count++;
  }

  velocities[vel_index] = new_val;
  vel_sum += new_val;

  vel_index = (vel_index + 1) % VEL_WINDOW_SIZE;
}
