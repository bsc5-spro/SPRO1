#ifndef OPTO

#define OPTO

void opto_init(void);

unsigned long
get_delta_ticks(void); // return time between two obstructions in ms
float get_delta_time(void);

unsigned char toggle_recording(void);

void zero_distance(void);
float get_distance_travelled(void);

float get_velocity(void);
float get_average_velocity(int n); // get rolling average with n-backsteps

void monitor_encoder(void);

#endif
