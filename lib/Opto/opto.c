#include "opto.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TIME_LENGTH sizeof(uint16_t)

static uint16_t previous_edge, current_edge;//, overflow_count;
static unsigned long delta_tick;

static uint16_t previous_tick;
// static unsigned char overflowed;

// char check_overflow(void);
// unsigned long overflow_ticks(char);

void opto_init(void)
{
  TCCR1A = 0x00; // pure ticks counter
  TCCR1B = 0xC5;

  previous_edge = ICR1;
  previous_tick = TCNT1;
  // overflow_count = 0;
  // overflowed = 0;
  delta_tick = 0;
}

unsigned long get_delta_ticks(void)
{
  // check_overflow();

  if (ICR1 != previous_edge)
  { // There has been a new
    // obstruction
    current_edge = ICR1;

    // if (current_edge > previous_edge)
    // {
    //   delta_tick = (unsigned long)(current_edge - previous_edge + overflow_ticks(0));
    // }
    // else
    // {
    //   delta_tick =
    //       (unsigned long)(current_edge + (pow(2, 16) - previous_edge) + overflow_ticks(-1));
    // }

    delta_tick = current_edge-previous_edge;

    // overflow_count = 0;
    previous_edge = current_edge;

    // return delta_tick / (F_CPU / 1000);
    return delta_tick;
  }
  return 0;
}

float get_delta_time(void){
  if (ICR1 != previous_edge){
    return (float)(get_delta_ticks()-1)/16;
  }
  return 0;
}

// unsigned long overflow_ticks(char delta) { 
//   unsigned long cnt;
//   if (delta < 0 && abs(delta) > overflow_count)
//     cnt = 0;
//   else
//     cnt = overflow_count+delta;
//   return cnt * pow(2,16);
// }

// char check_overflow(void)
// {
//   // printf("%u : %u\n", TCNT1, previous_tick);
//   if (TCNT1 < previous_tick)
//   {
//     if (!overflowed)
//     {
//       overflow_count++;
//       overflowed++;
//       return 1;
//     }
//   }
//   else
//   {
//     if (overflowed)
//     {
//       overflowed = 0;
//     }
//   }
//   previous_tick = TCNT1;
//   return 0;
// }
