#ifndef NEXTION
#define NEXTION

typedef struct
{
    int i_number;
    int decimalPlace;
    float f_number;
} fixed;


void init_display(void);
void set_speed_display(float speed);

void set_value(char component[], int val);
void set_property(char component[], char property[], int val);
int get_value(char component[]);
int read_value(void);

fixed read_numpad(void);

#endif