#include "nextion.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <usart.h>
#include <util/delay.h>

#define NUMBER_STRING 1001
#define SET 2001
#define SET_SCREEN 2002
#define CONTROL 3001

#define PACKET_LEN 5
#define RX_BUFFER_SIZE 32

volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_head = 0;
volatile uint8_t rx_tail = 0;

static char readValue;
volatile uint8_t control_packet_flag = 0;

// int read_value(void);

int uart_read_byte(uint8_t *byte);

void clear_buffer(void) {
  while (UCSR0A & (1 << RXC0)) {
    (void)UDR0;
  }
}

void init_display(void) {
  uart_init(); // open the communication to the microcontroller
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1
  sei();
  io_redirect(); // redirect input and output to the communication
  set_page(0);
  readValue = 0;
}

int get_value(char component[]) {
  printf("get %s.val%c%c%c", component, 255, 255, 255);
  return read_value();
}

void set_value(char component[], int val) {
  printf("%s.val=%d%c%c%c", component, val, 255, 255, 255);
}

void set_property(char component[], char property[], int val) {
  printf("%s.%s=%d%c%c%c", component, property, val, 255, 255, 255);
}

void set_page(int index) {
  printf("page %d%c%c%c", index, 255, 255, 255); // init at 9600 baud.
  _delay_ms(20);
}

void echo_serial(void) {
  // char input;
  while (1) {
    if (!(UCSR0A & (1 << RXC0))) {
      continue;
    }
    // scanf("%c", &input);
    printf("%c", UDR0);
  }
}
char read_value(void) {
  char readBuffer[20];
  int typeExpected = 0;
  readValue = 0;

  uint8_t byte;
  while (!uart_read_byte(&byte)) {
    ; // wait for incoming byte
  }

  readBuffer[0] = byte;

  switch (byte) {
  case 0x71:
    typeExpected = NUMBER_STRING;
    break;
  case 0x11:
    typeExpected = CONTROL;
    break;
  case 0x33:
    typeExpected = SET;
    break;
  case 0x32:
    typeExpected = SET_SCREEN;
    break;
  case 0x1A:
    for (int j = 0; j < 3; j++) {
      while (!uart_read_byte(&byte))
        ; // clear out tail
    }
    break;
  default:
    break;
  }

  int packetLength = (typeExpected == NUMBER_STRING) ? 8 : 5;
  for (int i = 1; i < packetLength; i++) {
    while (!uart_read_byte(&byte))
      ;
    readBuffer[i] = byte;
  }

  switch (typeExpected) {
  case NUMBER_STRING:
    if (readBuffer[0] == 0x71 && readBuffer[5] == -1 && readBuffer[6] == -1 &&
        readBuffer[7] == -1) // This is a complete number return
    {
      readValue = readBuffer[1] | (readBuffer[2] << 8) | (readBuffer[3] << 16) |
                  (readBuffer[4] << 24);
    }
    break;
  case SET:
    if (readBuffer[0] == 0x33 && readBuffer[2] == -1 && readBuffer[3] == -1 &&
        readBuffer[4] == -1) // This is a complete number return
    {
      readValue = readBuffer[1];
    }
    break;
  case SET_SCREEN:
    if (readBuffer[0] == 0x32 && readBuffer[2] == -1 && readBuffer[3] == -1 &&
        readBuffer[4] == -1) // This is a complete number return
    {
      readValue = readBuffer[1];
    }
    break;
  case CONTROL:
    if (readBuffer[0] == 0x11 && readBuffer[2] == -1 && readBuffer[3] == -1 &&
        readBuffer[4] == -1) // This is a complete number return
    {
      readValue = readBuffer[1];
      if (readValue == 1) { // start button
        return 0xa;
      } else if (readValue == 2) { // back button
        return 0xb;
      } else if (readValue == 0x00) { // restart button
        return 0xc;
      }
    }
    break;

  default:
    break;
  }

  return readValue;
}

fixed read_numpad(void) {
  int numberBuffer[20];
  int index = 0;

  fixed number;
  number.i_number = 0;
  number.f_number = 0;
  number.decimalPlace = 0; // place of decimal point from the right

  char receivingNumbers = 1;

  while (receivingNumbers) {

    char action = read_value();
    switch (action) {
    case 0x11:
      numberBuffer[index] = -1;
      index++;
      break;
    case 0x12:
      index--;
      break;
    case 0xb:
      receivingNumbers = 0;
      break;
    default:
      numberBuffer[index] = action;
      index++;
      break;
    }

    int multiplier = 1.0;
    int count = 0;
    number.i_number = 0;
    for (int i = index - 1; i >= 0; i--) {
      if (numberBuffer[i] == -1) {
        number.decimalPlace = count;
      } else {
        number.i_number += (float)numberBuffer[i] * multiplier;
        multiplier *= 10;
        count++;
      }
    }

    set_value("inpnum", number.i_number);
    set_property("inpnum", "vvs1", number.decimalPlace);
  }

  if (number.decimalPlace == 0)
    number.f_number = (float)number.i_number;
  else
    number.f_number = (float)number.i_number / pow(10, number.decimalPlace);

  return number;
}

void update_run_screen(uint16_t distance, uint16_t time, uint16_t velocity) {
  // update the each component on the screen
  // (in proper units)

  set_property("rundistance", "val", distance);
  set_property("rundistance", "vvs1", 1); // mm -> cm

  set_property("runtime", "val", time);
  set_property("runtime", "vvs1", 2); // 1/100 s -> s

  set_property("runvelocity", "val", velocity);
  set_property("runvelocity", "vvs1", 1);
}

void update_main_page(fixed distance, fixed time) {
  set_value("distance", distance.i_number);
  set_property("distance", "vvs1", distance.decimalPlace);
  set_value("time", time.i_number);
  set_property("time", "vvs1", time.decimalPlace);
}

// From ChatGPT
int check_run_end(void) {
  if (control_packet_flag) {
    control_packet_flag = 0;
    return 1;
  }
  return 0;
}

ISR(USART_RX_vect) {
  uint8_t next = (rx_head + 1) % RX_BUFFER_SIZE;
  uint8_t byte = UDR0; // Read the byte immediately

  if (next != rx_tail) { // prevent overflow
    rx_buffer[rx_head] = byte;
    rx_head = next;
  }

  static const uint8_t expected[PACKET_LEN] = {0x11, 0x00, 0xFF, 0xFF, 0xFF};
  static uint8_t idx = 0;

  if (byte == expected[idx]) {
    idx++;
    if (idx == PACKET_LEN) {
      control_packet_flag = 1;
      idx = 0;
    }
  } else {
    idx = (byte == expected[0]) ? 1 : 0;
  }
}

int uart_read_byte(uint8_t *byte) {
  if (rx_tail == rx_head) {
    return 0; // no data available
  }

  *byte = rx_buffer[rx_tail];
  rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
  return 1; // byte returned
}
