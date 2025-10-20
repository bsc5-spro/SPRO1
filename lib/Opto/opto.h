#ifndef OPTO

#define OPTO

void opto_init(void);
unsigned long get_delta_ticks(void); // return time between two obstructions in ms
float get_delta_time(void);

// TODO: only for debug
unsigned long overflow_ticks(char delta);

// unsigned int get_n_average(int n);

#endif
