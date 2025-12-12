#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#include <motor.h>
#include <nextion.h>
#include <opto.h>

static uint16_t targetDistance; // in mm
static uint16_t targetTime;     // in tenth of a sec

static uint16_t vError;

void reset_run(void);

int main(void) {

  DDRB &= ~0x01; // all pins act as input
  PORTB |= 0x01; // all pins in pull-down mode

  vError = 1;

  // SETUP //

  opto_init();
  reset_run();

  init_display();

  char runSelected = 0;
  while (!runSelected) {
    int action = read_value();
    switch (action) {
    case 1: {
      fixed number = read_numpad();
      targetDistance = (uint16_t)(number.f_number * 10);
      set_value("distance", number.i_number);
      set_property("distance", "vvs1", number.decimalPlace);
      break;
    }
    case 2: {
      fixed number = read_numpad();
      targetTime = (uint16_t)(number.f_number * 10);
      set_value("time", number.i_number);
      set_property("time", "vvs1", number.decimalPlace);
      break;
    }
    case 0:
      // only continue on if both dist and time are nonzero
      if (targetDistance != 0 && targetTime != 0) {
        runSelected = 1;
        set_page(2);
      }
      break;

    default:
      break;
    }
  }

  // CONTROL //

  toggle_recording();

  char currentPWM = 80;
  // TODO: change according to tests for average run
  pwm1_set_duty(currentPWM); // initial acceleration

  int velocityDeviation = get_average_velocity() - get_current_velocity();

  char loopCount = 0;

  while (get_distance_travelled() < targetDistance) {
    loopCount++;
    monitor_encoder();

    if (loopCount % 6 == 0) {
      // check if velocity stabilized
      if (abs(velocityDeviation) < vError) {
        uint16_t distanceRemaining = targetDistance - get_distance_travelled();
        uint16_t timeRemaining = targetTime - get_total_time();

        uint16_t requiredVelocity = distanceRemaining / timeRemaining;

        int deltaV = requiredVelocity - get_average_velocity();

        if (abs(deltaV) < vError) {
          continue;
        } else if (deltaV < 0) { // TODO: make proportional
          currentPWM -= 10;
        } else {
          currentPWM += 10;
        }
        pwm1_set_duty(currentPWM);
      }
    }

    if (loopCount % 15 == 0) {
      loopCount = 0;
      update_run_screen(get_distance_travelled(), get_total_time(),
                        get_average_velocity());
    }
  }

  pwm1_set_duty(0);

  // END RUN //
  return 0;
}

void reset_run(void) {
  targetDistance = 0;
  targetTime = 0;

  zero_distance();
  zero_time();
}
