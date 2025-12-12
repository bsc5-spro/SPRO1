#ifndef NEXTION
#define NEXTION

#include <stdint.h> // for uint16_t

typedef struct {
  int i_number;
  int decimalPlace;
  float f_number;
} fixed;

void init_display(void);

void set_value(char component[], int val);
void set_property(char component[], char property[], int val);

void set_page(int index);

int get_value(char component[]);
int read_value(void);

fixed read_numpad(void);

void update_run_screen(uint16_t rundistance, uint16_t runtime,
                       uint16_t runvelocity);

#endif
