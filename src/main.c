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

float distance;
float time;

int main(void) {
  uart_init();   // open the communication to the microcontroller
  io_redirect(); // redirect input and output to the communication

  init_display();

  char runSelected = 0;
  while (!runSelected){
    int action = read_value();
    switch (action)
    {
    case 1:
      distance = read_numpad();
      break;
    case 2:
      time = read_numpad();
      break;
    case 0;
      // run;
      runSelected = 1;
      break;
    
    default:
      break;
    }
  }

  return 0;
}
