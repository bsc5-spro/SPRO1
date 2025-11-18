#ifndef MOTOR_CONTROL
#define MOTOR_CONTROL

#include <stdint.h>

/**
 * @brief Initialises PWM generation with a frequency of approx. 4 KHz at PB1
 * (Fast mode, Timer1)
 **/
void pwm1_init(void);
void pwm1_set_duty(unsigned char input);

void motor_forward(float speed);
void motor_stop(void); // same as motor_forward(0)

#endif
