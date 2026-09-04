#include "simpletools.h"
#include "config.h"
#include "motors.h"
#include "imu.h"
#include "modes.h"

#define TWIST_THRESH    90
#define NEUTRAL_RATE    5
#define CONFIRM_MS      1000

int arc_tol[]   = {ARC_TOL_B,   ARC_TOL_I,   ARC_TOL_A};
int tempo_tol[] = {TEMPO_TOL_B, TEMPO_TOL_I, TEMPO_TOL_A};
int wall_tol[]  = {WALL_TOL_B,  WALL_TOL_I,  WALL_TOL_A};

int accumulated_angle = 0;
unsigned int prev_time_ticks = 0;

void indicate_selection(int count) {
    for (int i = 0; i <= count; i++) {
        motor_buzz(PIN_MOTOR_FRONT, BUZZ_SOFT, 80);
        pause(150);
    }
    pause(400);
}

int main() {
    // IMU first — single init, owns the bus
    imu_init();
    motors_init();

    print("\n=== PATHFINDER ===\n");
    print("Twist CW to cycle, hold still to confirm\n\n");

    motor_buzz(PIN_MOTOR_FRONT, BUZZ_SOFT, 100);
    pause(300);

    // --- MODE SELECTION ---
    int selected_mode = 0;
    unsigned int last_activity = CNT;
    int mode_confirmed = 0;
    prev_time_ticks = CNT;

    const char* mode_names[] = {"Sweeping", "Touch", "Shoreline", "Ground"};
    print("Mode: %s\n", mode_names[selected_mode]);

    while (!mode_confirmed) {
        int gyro_raw = imu_read_gyro_x() - cal_gyro_x;
        int gyro_x   = gyro_raw / GYRO_SCALE; 
        unsigned int now = CNT;
        unsigned int dt_ticks = now - prev_time_ticks;
        prev_time_ticks = now;

        int dt_ms = (int)(dt_ticks / (CLKFREQ / 1000));

        if (dt_ms > 0 && dt_ms < 100) {
            accumulated_angle += (gyro_x * dt_ms) / 1000;
        }

        if (gyro_x < NEUTRAL_RATE) {
            accumulated_angle = (accumulated_angle * 7) / 10;
        }

        if (accumulated_angle > TWIST_THRESH) {
            selected_mode = (selected_mode + 1) % 4;
            print("Mode: %s\n", mode_names[selected_mode]);
            indicate_selection(selected_mode);
            accumulated_angle = 0;
            last_activity = CNT;
        }

        if (abs(gyro_x) < NEUTRAL_RATE) {
            unsigned int elapsed = (CNT - last_activity) / (CLKFREQ / 1000);
            if (elapsed > CONFIRM_MS) {
                mode_confirmed = 1;
                print("Confirmed: %s\n\n", mode_names[selected_mode]);
                motor_buzz(PIN_MOTOR_FRONT, BUZZ_HARD, 300);
                pause(500);
            }
        } else {
            last_activity = CNT;
        }

        pause(20);
    }

    // --- DIFFICULTY SELECTION ---
    int selected_diff = 0;
    int diff_confirmed = 0;
    accumulated_angle = 0;
    prev_time_ticks = CNT;
    last_activity = CNT;

    const char* diff_names[] = {"Beginner", "Intermediate", "Advanced"};
    print("Difficulty: %s\n", diff_names[selected_diff]);

    while (!diff_confirmed) {
        int gyro_raw = imu_read_gyro_x() - cal_gyro_x;
        int gyro_x   = gyro_raw / GYRO_SCALE; 
        unsigned int now = CNT;
        unsigned int dt_ticks = now - prev_time_ticks;
        prev_time_ticks = now;

        int dt_ms = (int)(dt_ticks / (CLKFREQ / 1000));

        if (dt_ms > 0 && dt_ms < 100) {
            accumulated_angle += (gyro_x * dt_ms) / 1000;
        }

        if (gyro_x < NEUTRAL_RATE) {
            accumulated_angle = (accumulated_angle * 7) / 10;
        }

        if (accumulated_angle > TWIST_THRESH) {
            selected_diff = (selected_diff + 1) % 3;
            print("Difficulty: %s\n", diff_names[selected_diff]);
            indicate_selection(selected_diff);
            accumulated_angle = 0;
            last_activity = CNT;
        }

        if (abs(gyro_x) < NEUTRAL_RATE) {
            unsigned int elapsed = (CNT - last_activity) / (CLKFREQ / 1000);
            if (elapsed > CONFIRM_MS) {
                diff_confirmed = 1;
                print("Confirmed: %s\n\n", diff_names[selected_diff]);
                motor_buzz(PIN_MOTOR_FRONT, BUZZ_HARD, 300);
                pause(500);
            }
        } else {
            last_activity = CNT;
        }

        pause(20);
    }

    // --- CALIBRATION ---
    print("Calibrating... hold still\n");
    motor_buzz(PIN_MOTOR_FRONT, BUZZ_HARD, 500);
    pause(3000);
    imu_calibrate();

    // --- SESSION ---
    print("GO! 30 seconds...\n");
    int score = run_mode(selected_mode, selected_diff);
    result_buzz(score);

    print("Score: %d/%d\n", score, SCORE_INTERVALS);

    return 0;
}