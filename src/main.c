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

  init_display();

  char runSelected = 0;
  while (!runSelected) {
    int action = read_value();
    switch (action) {
    case 1: {
      fixed number = read_numpad();
      distance = number.f_number;
      set_value("distance", number.i_number);
      set_property("distance", "vvs1", number.decimalPlace);
      break;
    }
    case 2: {
      fixed number = read_numpad();
      time = number.f_number;
      set_value("time", number.i_number);
      set_property("time", "vvs1", number.decimalPlace);
      break;
    }
    case 0:
      // run;
      runSelected = 1;
      break;

    default:
      break;
    }
  }

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
