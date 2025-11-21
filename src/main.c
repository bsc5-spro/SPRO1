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
#include <usart.h>

#include <opto.h>
#include <nextion.h>

int main(void) {
  uart_init();   // open the communication to the microcontroller
  io_redirect(); // redirect input and output to the communication

  init_display();

  uint32_t x = get_value("x0");

  set_value("n0", x);

  return 0;
}
