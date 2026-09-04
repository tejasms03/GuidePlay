#include "simpletools.h"
#include "config.h"
#include "ultrasonic.h"
#include "motors.h"
#include "modes.h"
#include "imu.h" 

// --- SHORELINING ---
int mode_shoreline(int diff) {
    int tol = wall_tol[diff];
    int target = WALL_TARGET_CM;  // 30cm default
    int score = 0;
    
    print("Shorelining started. Target: %d+/-%d cm\n", target, tol);
    
    for (int t = 0; t < SCORE_INTERVALS; t++) {
        int left_dist = ultrasonic_left();
        int error = left_dist - target;
        
        if (left_dist < 0 || left_dist > 400) {
            pause(LOOP_INTERVAL_MS);
            continue;
        }
        
        if (error > tol) {
            motor_buzz(PIN_MOTOR_LEFT, 1, 80);  
        } 
        else if (error < -tol) {
            motor_buzz(PIN_MOTOR_LEFT, 2, 80);  
        } 
        else {
            score++;
        }
        
        if (diff == 2) {
            int right_dist = ultrasonic_right();
            
            if (right_dist >= 0 && right_dist <= 400) {
                int r_error = right_dist - target;
                
                if (r_error > tol) {
                    motor_buzz(PIN_MOTOR_RIGHT, 1, 80);  // Too far from right wall
                } 
                else if (r_error < -tol) {
                    motor_buzz(PIN_MOTOR_RIGHT, 2, 80);  // Too close to right wall
                }
                
                if (abs(error) <= tol && abs(r_error) <= tol) {
                } 
                else if (abs(error) <= tol && abs(r_error) > tol) {
                }
            }
        }
        
        pause(LOOP_INTERVAL_MS);  // 100ms
    }
    
    return score;
}

int mode_ground(int diff) {
    int score      = 0;
    int prev_front = ultrasonic_front();
    unsigned int touch_start = 0;
    int in_touch   = 0;

    for (int t = 0; t < SCORE_INTERVALS; t++) {
        
        // Read everything once at top
        int az = imu_read_accel_z() - cal_accel_z;
        int ax = imu_read_accel_x() - cal_accel_x;
        int gx = imu_read_gyro_x()  - cal_gyro_x;
        int fc = ultrasonic_front();

        // --- LIFT DETECTION ---
        int lifted = (az > LIFT_THRESHOLD || az < -LIFT_THRESHOLD);

        if (lifted) {
            motor_all_stutter();
        } else {

            // --- CURB DETECTION ---
            if (fc > 0 && fc < 400) {
                if ((prev_front - fc) > CURB_THRESHOLD)
                    motor_buzz(PIN_MOTOR_FRONT, BUZZ_HARD, 150);
            }

            // --- TOUCH-SLIDE (Intermediate+) ---
            // Use ax for tap (lateral impact when tip contacts surface)
            // Use gx for slide confirmation
            if (diff >= 1) {
                if (!in_touch && az < -TAP_THRESHOLD) {
                    in_touch    = 1;
                    touch_start = CNT;
                }
                if (in_touch) {
                    unsigned int held_ms = (CNT - touch_start) / (CLKFREQ / 1000);
                    if (held_ms >= TOUCH_HOLD_MS && abs(gx) > 200) {
                        motor_buzz(PIN_MOTOR_FRONT, BUZZ_SOFT, 100);
                        in_touch = 0;
                    }
                    if (held_ms > 4000) in_touch = 0;
                }
            }

            score++;
        }

        if (fc > 0 && fc < 400) prev_front = fc;
        pause(LOOP_INTERVAL_MS);
    }

    return score;
}

// --- SWEEPING ---
int mode_sweep(int diff) {
    int tol_tempo  = tempo_tol[diff];
    int target_tempo = SWEEP_TARGET_MS;   // 800ms
    int arc_thresh = 300;                 // raw gyro deg/s — tune this
                                          // 300 ≈ moderate sweep speed
    int score      = 0;
    int last_gyro  = 0;
    int peak_gyro  = 0;                   // track peak between crossings

    unsigned int last_peak_ticks = CNT;   // unsigned — no overflow

    print("Sweeping started.\n");

    for (int t = 0; t < SCORE_INTERVALS; t++) {
        int gyro = imu_read_gyro_x() - cal_gyro_x;

        // Track peak magnitude during this half-sweep
        if (abs(gyro) > abs(peak_gyro)) peak_gyro = gyro;

        // Zero crossing = end of one half-sweep
        int crossed = (last_gyro > 50 && gyro < -50) ||
                      (last_gyro < -50 && gyro > 50);
        // Use 50 as deadband — avoids false crossings at rest

        if (crossed) {
            unsigned int now    = CNT;
            unsigned int ticks  = now - last_peak_ticks;
            int tempo_ms        = (int)(ticks / (CLKFREQ / 1000));
            last_peak_ticks     = now;

            int tempo_err = abs(tempo_ms - target_tempo);
            int arc_ok    = abs(peak_gyro) > arc_thresh;
            int tempo_ok  = tempo_err <= tol_tempo;

            if (arc_ok && tempo_ok) {
                score++;
            } else {
                if (!arc_ok) {
                    // Arc too small — buzz the side that just finished
                    if (last_gyro > 0)
                        motor_buzz(PIN_MOTOR_RIGHT, BUZZ_SOFT, 80);
                    else
                        motor_buzz(PIN_MOTOR_LEFT,  BUZZ_SOFT, 80);
                }
                if (!tempo_ok) {
                    motor_buzz(PIN_MOTOR_FRONT, BUZZ_SOFT, 80);
                }
            }

            peak_gyro = 0;  // reset for next half-sweep
        }

        last_gyro = gyro;
        pause(LOOP_INTERVAL_MS);
    }

    return score;
}

// --- TOUCH RHYTHM ---
int mode_touch(int diff) {
    int bpm      = (diff == 0) ? 60 : (diff == 1) ? 75 : 90;
    int beat_ms  = 60000 / bpm;       // ms per beat: 1000 / 800 / 667
    int tol_ms   = tempo_tol[diff];   // timing window: 300 / 200 / 80ms
    int three_pt = (diff >= 1);       // 3-point starts at intermediate
    int steps    = three_pt ? 3 : 2;
    int score    = 0;

    int expected    = 0;              // 0=left, 1=right, 2=probe
    int rhythm_ok   = 1;             // stays 1 while hitting beats correctly
    unsigned int beat_start = CNT;

    print("Touch Rhythm. BPM:%d Tol:%dms %s\n",
           bpm, tol_ms, three_pt ? "3-pt" : "2-pt");

    for (int t = 0; t < SCORE_INTERVALS; t++) {

        // Read sensors once
        int ax = imu_read_accel_x() - cal_accel_x;  // lateral tap axis
        int fc = ultrasonic_front();

        // Elapsed time since last beat
        unsigned int elapsed_ticks = CNT - beat_start;
        int beat_elapsed = (int)(elapsed_ticks / (CLKFREQ / 1000));

        // On-beat window
        int on_beat      = (beat_elapsed >= (beat_ms - tol_ms)) &&
                           (beat_elapsed <= (beat_ms + tol_ms));

        // Tap and probe detection
        int tapped_left  = (ax < -TAP_THRESHOLD);
        int tapped_right = (ax >  TAP_THRESHOLD);
        int probed       = (fc > 0 && fc < PROBE_DIST_CM);

        // Check for correct hit
        int hit = 0;
        if (expected == 0 && tapped_left  && on_beat) hit = 1;
        if (expected == 1 && tapped_right && on_beat) hit = 1;
        if (expected == 2 && probed       && on_beat) hit = 1;

        if (hit) {
            // Confirm with matching motor
            if (expected == 0) motor_buzz(PIN_MOTOR_LEFT,  BUZZ_SOFT, 60);
            if (expected == 1) motor_buzz(PIN_MOTOR_RIGHT, BUZZ_SOFT, 60);
            if (expected == 2) motor_buzz(PIN_MOTOR_FRONT, BUZZ_SOFT, 60);

            expected   = (expected + 1) % steps;
            beat_start = CNT;
            rhythm_ok  = 1;
        }

        // Missed beat — past tolerance window
        if (beat_elapsed > beat_ms + tol_ms) {
            motor_buzz(PIN_MOTOR_FRONT, BUZZ_HARD, 80);
            expected   = (expected + 1) % steps;
            beat_start = CNT;
            rhythm_ok  = 0;   // bad rhythm until next successful hit
        }

        // Score per interval — consistent with all other modes
        if (rhythm_ok) score++;

        pause(LOOP_INTERVAL_MS);
    }

    return score;
}

// --- MODE DISPATCHER ---
int run_mode(int mode, int diff) {
    switch(mode) {
        case 0: 
            return mode_sweep(diff);      
        case 1:
            return mode_touch(diff); 
        case 2:
            return mode_shoreline(diff);  
        case 3:
            return mode_ground(diff);     
        default:
            return 0;
    }
}