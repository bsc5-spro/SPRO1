/*
 * TITLE
 *
 * Created: Thu, 02 Oct 2025 08:47:47 GMT
 * Author : Adam
 */

#include <avr/io.h>
#include <limits.h>
#include <stdio.h>
#include <util/delay.h>

#include "usart.h"

int main(void) {
  uart_init();   // open the communication to the microcontroller
  io_redirect(); // redirect input and output to the communication

  TCCR1A = 0x00; // pure ticks counter
  TCCR1B = 0xC5;

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

  uint16_t previous_edge, current_edge, overflow_cnt;
  float d_time;
  unsigned int d_cnt, count;

  int overflowed;

  previous_edge = ICR1;
  count = 0;

  while (1) {
    if (ICR1 != previous_edge) {
      previous_edge = ICR1;
      count++;
      printf("There were %d obstructions.\n", count);
    }
  }

  return 0;
}
