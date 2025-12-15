#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>

#include <motor.h>
#include <nextion.h>
#include <opto.h>

static uint16_t targetDistance; // in mm
static uint16_t targetTime;     // in hundredth of a sec

static uint16_t vError;

void reset_run(void);

int main(void) {

  vError = 1;

  // SETUP //

  opto_init();

  _delay_ms(4000);
  init_display();
  _delay_ms(200);
  clear_buffer();

  pwm1_init();

  fixed distance, time;

  distance.f_number = 400.0;
  distance.i_number = 400;
  distance.decimalPlace = 0;

  time.f_number = 50.5;
  time.i_number = 505;
  time.decimalPlace = 1;

  targetDistance = (uint16_t)(distance.f_number * 10);
  targetTime = (uint16_t)(time.f_number * 10);

  // echo_serial();

  while (1) {
    reset_run();
    update_main_page(distance, time);

    char runSelected = 0;
    while (!runSelected) {
      char action = read_value();
      // printf("%c%c%c%c", action, 255, 255, 255);
      switch (action) {
      case 0x1: {
        // printf("01%c%c%c", 255, 255, 255);
        distance = read_numpad();
        targetDistance = (uint16_t)(distance.f_number * 10); // cm -> mm
        update_main_page(distance, time);
        break;
      }
      case 0x2: {
        time = read_numpad();
        targetTime = (uint16_t)(time.f_number * 100); // s -> 1/100 s
        update_main_page(distance, time);
        break;
      }
      case 0xa:
        // only continue on if both dist and time are nonzero
        if (targetDistance != 0 && targetTime != 0) {
          runSelected = 1;
          set_page(2);
        }
        break;

      default:
        break;
      }
      _delay_ms(250);
    }

    // int fake_distance = 0;
    // int fake_time = 0;
    // while (1) {
    //   update_run_screen(fake_distance, fake_time, 9000);
    //   fake_distance += 5;
    //   fake_time += 1;
    //   _delay_ms(500);
    // }

    // CONTROL //

    start_recording();

    char currentPWM = 80;
    // TODO: change according to tests for average run
    pwm1_set_duty(currentPWM); // initial acceleration

    int velocityDeviation;

    uint8_t loopCount = 0;

    while (get_distance_travelled() < targetDistance) {
      if (check_run_end()) {
        break;
      }
      loopCount++;
      monitor_encoder();

      if (loopCount % 6 == 0) {
        // check if velocity stabilized
        velocityDeviation = get_average_velocity() - get_current_velocity();
        if (abs(velocityDeviation) < vError) {
          uint16_t distanceRemaining =
              targetDistance - get_distance_travelled();          // mm
          uint16_t timeRemaining = targetTime - get_total_time(); // 1/100 s
          timeRemaining /= 100; // convert from 1/100 s to s

          uint16_t requiredVelocity =
              distanceRemaining / timeRemaining; //      mm / s

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
      // _delay_ms(10);
    }

    // END RUN //
    pwm1_set_duty(0);
    stop_recording();
    update_run_screen(get_distance_travelled(), get_total_time(), 0);
    update_run_screen(get_distance_travelled(), get_total_time(), 90);

    char reset = 0;
    while (!reset) {
      if (read_value() == 0xc) {
        set_page(0);
        reset = 1;
      }
    }
  }
  return 0;
}

void reset_run(void) {
  // targetDistance = 0;
  // targetTime = 0;

  zero_distance();
  zero_time();
}
