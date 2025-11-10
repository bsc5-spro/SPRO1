#ifndef OPTO

#define OPTO

#include <stdint.h>
void opto_init(void);

// return the number of ticks between two obstructions.
uint16_t get_delta_ticks(void);

unsigned char toggle_recording(void);

void zero_distance(void);
uint16_t get_distance_travelled(void);

uint16_t get_average_velocity(void);

void monitor_encoder(void);

#endif
