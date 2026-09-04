#include "simpletools.h"
#include "config.h"

int read_cm(int trig_pin, int echo_pin) {
    low(trig_pin);
    pause(2);
    
    high(trig_pin);
    pause(10);
    low(trig_pin);
    
    long duration = pulse_in(echo_pin, 1);
    
    if (duration <= 0) return 400;
    
    return (int)(duration / 58);
}

int ultrasonic_left()  { return read_cm(PIN_US_LEFT_TRIG,  PIN_US_LEFT_ECHO);  }
int ultrasonic_right() { return read_cm(PIN_US_RIGHT_TRIG, PIN_US_RIGHT_ECHO); }
int ultrasonic_front() { return read_cm(PIN_US_FRONT_TRIG, PIN_US_FRONT_ECHO); }