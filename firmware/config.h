#ifndef CONFIG_H
#define CONFIG_H

// ================================================================
// PIN ASSIGNMENTS
// ================================================================

// Vibration motors 
#define PIN_MOTOR_LEFT      0
#define PIN_MOTOR_RIGHT     1
#define PIN_MOTOR_FRONT     2

// Ultrasonic sensors — trigger and echo for each
#define PIN_US_FRONT_TRIG    3
#define PIN_US_FRONT_ECHO    4
#define PIN_US_LEFT_TRIG   5
#define PIN_US_LEFT_ECHO   6
#define PIN_US_RIGHT_TRIG   7
#define PIN_US_RIGHT_ECHO   8

// Buttons (use Activity Board's built-in P26/P27 if available
#define PIN_BTN_MODE_UP     14
#define PIN_BTN_MODE_DN     15
#define PIN_BTN_DIFF        16
#define PIN_BTN_START       17

// IMU I2C — Activity Board hardwires I2C to P28/P29
#define I2C_SCL             9
#define I2C_SDA             10
#define IMU_ADDR            0x68    

// ================================================================
// DIFFICULTY THRESHOLDS
// Index 0 = Beginner, 1 = Intermediate, 2 = Advanced
// ================================================================

// Sweeping — arc tolerance in degrees, tempo tolerance in ms
#define ARC_TOL_B           25
#define ARC_TOL_I           15
#define ARC_TOL_A           8

#define TEMPO_TOL_B         300
#define TEMPO_TOL_I         200
#define TEMPO_TOL_A         80

// Shorelining — proximity band tolerance in cm
#define WALL_TOL_B          15
#define WALL_TOL_I          10
#define WALL_TOL_A          8

// Shorelining — target distance from wall in cm
#define WALL_TARGET_CM      30

// ================================================================
// SENSOR DETECTION THRESHOLDS
// ================================================================
// Ground reading — raw IMU accel units that count as a lift event
#define LIFT_THRESHOLD      500

// Ground reading — cm drop on front sensor that counts as a curb
#define CURB_THRESHOLD      10

// Touch rhythm — raw accel_x magnitude that counts as a tap
#define TAP_THRESHOLD       300

// Touch-slide — minimum ms the tip must be held down before sliding
#define TOUCH_HOLD_MS       150

// Touch rhythm — landmark probe distance in cm 
#define PROBE_DIST_CM       20

// Sweeping — target half-sweep duration in ms 
#define SWEEP_TARGET_MS     800

#define GYRO_SCALE          131

// ================================================================
// SESSION AND SCORING
// ================================================================

#define SESSION_MS          30000   
#define LOOP_INTERVAL_MS    100     
#define SCORE_INTERVALS     300    

#define SCORE_GOOD          210  
#define SCORE_PARTIAL       150    

// ================================================================
// MOTOR BUZZ LEVELS
// Used as arguments to motor_buzz()
// ================================================================

#define BUZZ_OFF            0
#define BUZZ_SOFT           1
#define BUZZ_HARD           2
#define BUZZ_STUTTER        3

// ================================================================
// MODE IDs
// ================================================================

#define MODE_SWEEP          0
#define MODE_TOUCH          1
#define MODE_SHORELINE      2
#define MODE_GROUND         3

// Wrist twist detection 
#define TWIST_THRESHOLD     8000   
#define NEUTRAL_THRESHOLD   2000    
#define TWIST_DEBOUNCE_MS   500     
#define SELECT_TIMEOUT_MS   2500   

// Twist direction 
#define TWIST_CW            1      
#define TWIST_CCW          -1       

#endif