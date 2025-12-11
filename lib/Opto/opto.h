#ifndef OPTO

#define OPTO

#include <stdint.h>
void opto_init(void);

// return the number of ticks between two obstructions.
uint16_t get_delta_ticks(void);

unsigned char toggle_recording(void);

void zero_distance(void);
void zero_time(void);
uint16_t get_distance_travelled(void); // in mm
uint16_t get_total_time(void);

uint16_t get_current_velocity(void); // may be noisy, inaccurate

// TODO: make average window dynamically sized if needed
uint16_t get_average_velocity(void); // more accurate measure of velocity

void monitor_encoder(void);

#endif
