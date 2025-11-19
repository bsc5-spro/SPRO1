/*
 * TITLE
 *
 * Created: Thu, 02 Oct 2025 08:47:47 GMT
 * Author : Adam
 */

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include <motor.h>

#include "motor.h"
#include "usart.h"

int main(void) {
  uart_init();
  io_redirect();

  pwm1_init();
  printf("\nPWM set up.");

  motor_forward(0.2);
  printf("\nmotor speed set to 0.2");
  printf("\nOCR0A=%x", OCR0A);

  unsigned char input;

  while (1) {
    scanf("%hhi", &input);
    pwm1_set_duty(input);
    printf("\nOCR0A=%x", OCR0A);
  }

  return 0;
}
