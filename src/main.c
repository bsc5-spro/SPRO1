/*
 * TITLE
 *
 * Created: Thu, 02 Oct 2025 08:47:47 GMT
 * Author : Adam
 */

#include <avr/io.h>
// #include <limits.h>
#include <stdio.h>
#include <util/delay.h>

#include "usart.h"
#include <opto.h>

int main(void) {
  uart_init();   // open the communication to the microcontroller
  io_redirect(); // redirect input and output to the communication

  DDRB &= ~0x01; // all pins act as input
  PORTB |= 0x01; // all pins in pull-down mode

  /* PROCESS
   *
   * ICR1: holds the time of last rising edge (start of obstruction)
   * TCNT1: continuous count up to 2^16
   *
   * 1. See if ICR1 is updated
   * 2. see the difference between it and previous rising edge
   *  a. if the current rising edge was before the previous, overflow happened
   *  b. if overflow,
   *
   */

  opto_init();

  float dtime;

  while (1) {
    dtime = get_delta_time();
    if (dtime != 0)
      printf("Time elapsed: %.2f\n", dtime);
  }

  return 0;
}
