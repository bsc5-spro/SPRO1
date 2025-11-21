#ifndef NEXTION
#define NEXTION

void init_display(void);
void set_speed_display(float speed);

void set_value(char component[], int val);
int get_value(char component[]);

#endif