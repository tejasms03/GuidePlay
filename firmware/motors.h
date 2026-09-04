#ifndef MOTORS_H
#define MOTORS_H

void motors_init();
void motor_buzz(int pin, int level, int duration_ms);
void motor_all_stutter();
void result_buzz(int score);

#endif