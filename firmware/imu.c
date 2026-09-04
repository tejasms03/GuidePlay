#include "simpletools.h"
#include "simplei2c.h"
#include "config.h"
#include "imu.h"

i2c imu_bus;

// Calibration storage for all 6 axes
int cal_accel_x = 0;
int cal_accel_y = 0;
int cal_accel_z = 0;
int cal_gyro_x = 0;
int cal_gyro_y = 0;
int cal_gyro_z = 0;

static int read_byte(int reg) {
    int val;
    i2c_start(&imu_bus);
    i2c_writeByte(&imu_bus, IMU_ADDR << 1);
    i2c_writeByte(&imu_bus, reg);
    i2c_start(&imu_bus);
    i2c_writeByte(&imu_bus, (IMU_ADDR << 1) | 1);
    val = i2c_readByte(&imu_bus, 1);
    i2c_stop(&imu_bus);
    return val;
}

static int read_word(int reg) {
    int high = read_byte(reg);
    int low  = read_byte(reg + 1);
    return (short)((high << 8) | low);
}

void imu_init() {
    i2c_open(&imu_bus, I2C_SCL, I2C_SDA, 0);
    
    // Wake MPU6050
    i2c_start(&imu_bus);
    i2c_writeByte(&imu_bus, IMU_ADDR << 1);
    i2c_writeByte(&imu_bus, 0x6B);
    i2c_writeByte(&imu_bus, 0x00);
    i2c_stop(&imu_bus);
    
    pause(50);
    
    int who = read_byte(0x75);
    if (who != 0x68 && who != 0x69) {
        print("ERROR: MPU6050 not found! WHO_AM_I=0x%02X\n", who);
        while(1);
    }
    print("MPU6050 initialized (0x%02X)\n\n", who);
}

void imu_calibrate() {
    print("Calibrating... hold still...\n");
    
    long sum_ax = 0, sum_ay = 0, sum_az = 0;
    long sum_gx = 0, sum_gy = 0, sum_gz = 0;
    
    // Average 50 samples
    for (int i = 0; i < 50; i++) {
        sum_ax += read_word(0x3B);  // ACCEL_XOUT_H
        sum_ay += read_word(0x3D);  // ACCEL_YOUT_H
        sum_az += read_word(0x3F);  // ACCEL_ZOUT_H
        sum_gx += read_word(0x43);  // GYRO_XOUT_H
        sum_gy += read_word(0x45);  // GYRO_YOUT_H
        sum_gz += read_word(0x47);  // GYRO_ZOUT_H
        pause(10);
    }
    
    cal_accel_x = (int)(sum_ax / 50);
    cal_accel_y = (int)(sum_ay / 50);
    cal_accel_z = (int)(sum_az / 50);
    cal_gyro_x  = (int)(sum_gx / 50);
    cal_gyro_y  = (int)(sum_gy / 50);
    cal_gyro_z  = (int)(sum_gz / 50);
    
    print("Calibration complete:\n");
    print("  Accel X:%d Y:%d Z:%d\n", cal_accel_x, cal_accel_y, cal_accel_z);
    print("  Gyro  X:%d Y:%d Z:%d\n\n", cal_gyro_x, cal_gyro_y, cal_gyro_z);
}

int imu_read_accel_x() { return read_word(0x3B); }
int imu_read_accel_y() { return read_word(0x3D); }
int imu_read_accel_z() { return read_word(0x3F); }
int imu_read_gyro_x()  { return read_word(0x43); }
int imu_read_gyro_y()  { return read_word(0x45); }
int imu_read_gyro_z()  { return read_word(0x47); }