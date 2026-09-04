#include "simpletools.h"
#include "config.h"

void motors_init() {
    low(PIN_MOTOR_LEFT);
    low(PIN_MOTOR_RIGHT);
    low(PIN_MOTOR_FRONT);
}

void motor_buzz(int pin, int level, int duration_ms) {
    if (level == 0) {  // BUZZ_OFF
        low(pin); 
        return;
    }
    if (level == 1) {  // BUZZ_SOFT
        high(pin); 
        pause(duration_ms / 2); 
        low(pin);
    }
    else if (level == 2) {  // BUZZ_HARD
        high(pin); 
        pause(duration_ms); 
        low(pin);
    }
    else if (level == 3) {  // BUZZ_STUTTER
        for (int i = 0; i < 3; i++) {
            high(pin); 
            pause(50); 
            low(pin); 
            pause(50);
        }
    }
}

void motor_all_stutter() {
    motor_buzz(PIN_MOTOR_LEFT,  3, 0);
    motor_buzz(PIN_MOTOR_RIGHT, 3, 0);
    motor_buzz(PIN_MOTOR_FRONT, 3, 0);
}

void result_buzz(int score) {
    int pulses = (score >= 210) ? 3 : (score >= 150) ? 2 : 1;
    for (int i = 0; i < pulses; i++) {
        motor_buzz(PIN_MOTOR_FRONT, 2, 200);
        pause(200);
    }
}