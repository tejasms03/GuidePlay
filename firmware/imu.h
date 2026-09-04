#ifndef IMU_H
#define IMU_H

#include "simplei2c.h"

// Calibration baselines
extern int cal_accel_x;
extern int cal_accel_y;
extern int cal_accel_z;
extern int cal_gyro_x;
extern int cal_gyro_y;
extern int cal_gyro_z;

// Functions
void imu_init();
void imu_calibrate();

// Accelerometer 
int imu_read_accel_x();
int imu_read_accel_y();
int imu_read_accel_z();

// Gyroscope 
int imu_read_gyro_x();
int imu_read_gyro_y();
int imu_read_gyro_z();

// Helper to convert raw gyro to degrees/sec 
#define GYRO_TO_DEG(raw) ((raw) / 131)      
#define ACCEL_TO_G(raw)  ((raw) / 16384)    

#endif