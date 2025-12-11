/*
 * TITLE
 *
 * Created: Thu, 02 Oct 2025 08:47:47 GMT
 * Author : Adam
 */

#include <avr/io.h>
// #include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "usart.h"
#include <motor.h>
#include <opto.h>

static uint16_t targetDistance; // in mm
static uint16_t targetTime;

void reset_run(void);

int main(void) {
  uart_init(); // open the communication to the microcontroller
  // io_redirect(); // redirect input and output to the communication

  DDRB &= ~0x01; // all pins act as input
  PORTB |= 0x01; // all pins in pull-down mode

  // SETUP //

  opto_init();
  reset_run();

  // CONTROL //

  while (get_distance_travelled() < targetDistance) {
  }

  // END RUN //
  return 0;
}

void reset_run(void) {
  targetDistance = 4000;
  targetTime = 3000;

  zero_distance();
  zero_time();
}
