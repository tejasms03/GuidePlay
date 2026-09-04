#ifndef ULTRASONIC_H
#define ULTRASONIC_H

int read_cm(int trig_pin, int echo_pin);
int ultrasonic_left();
int ultrasonic_right();
int ultrasonic_front();

#endif