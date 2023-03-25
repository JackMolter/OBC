// Code for the IMU on the SatFab OBC

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "IMU.h"

uint8_t imu_gyro[6]; // stores gyro data

int16_t x_gyro;
int16_t y_gyro;
int16_t z_gyro;
float X_gyro;
float Y_gyro;
float Z_gyro;
int16_t* Xgyro;
int16_t* Ygyro;
int16_t* Zgyro;

uint8_t imu_accel[6]; // stores accelerometer data
int16_t x_accel;
int16_t y_accel;
int16_t z_accel;
float X_accel;
float Y_accel;
float Z_accel;
int16_t* Xaccel;
int16_t* Yaccel;
int16_t* Zaccel;

int16_t* bmi088_temp;


uint8_t gyro_val = 0x02;
uint8_t accel_val = 0x12;

// Initialize IMU 
void BMI088_init() {

    // configure the I2C Communication
    i2c_init(I2C_PORT, BMI088_BAUD);
    gpio_set_function(BMI088_SDA, GPIO_FUNC_I2C);
    gpio_set_function(BMI088_SCL, GPIO_FUNC_I2C);

    uint8_t data[2];

    // Reset IMU
    // Sending 5.3.16 ACC_SOFTRESET command
    //data[0] = 0x7E; 
    //data[1] = ACC_SOFTRESET;
    //i2c_write_blocking(I2C_PORT, addr_accel, data, 2, true);
    //sleep_ms(50);

    // turn accelerometer on (ACC_PWR_CTRL)
    data[0] = 0x7D;
    data[1] = 0x04;  // accelerometer on
    i2c_write_blocking(I2C_PORT, addr_accel, data, 2, true);
    sleep_ms(50);

    // set range 
    data[0] = 0x41;
    data[1] = ACC_RANGE;  // +- 6g 
    i2c_write_blocking(I2C_PORT, addr_accel, data, 2, true);
    sleep_ms(50);

    // Reset Gyroscope
    //data[0] = 0x14;
    //data[1] = GYRO_SOFTRESET;
    //i2c_write_blocking(I2C_PORT, addr_gyro, data, 2, true);
    //sleep_ms(50);

    // Set Gyroscope Range
    data[0] = 0x0F;
    data[1] = GYRO_RANGE; // range of +/- 2000 degrees/sec
    i2c_write_blocking(I2C_PORT, addr_gyro, data, 2, true);
    sleep_ms(50);

    // Set ODR
    //data[0] = ACC_CONF;
    //data[1] = NORMAL_ODR;
    //i2c_write_blocking(I2C_PORT, addr_accel, data, 2, true);
    //sleep_ms(50);

    // set sampling mode 
    //data[0] = 0x40;
    //data[1] = 0xA7;  // normal mode 
    //i2c_write_blocking(I2C_PORT, addr, data, 2, true);
    //sleep_ms(30);


    // turn gyro off
    //data[0] = 0x11;// GYRO_LPM1 register
    //data[1] = 0x80; // suspend mode 
    //i2c_write_blocking(I2C_PORT, addr, data, 2, true);
    //sleep_ms(30);

}

void BMI088_get_temp() {
    //Temperature Sensor Data Collection
    uint16_t temperature_unsigned;
    int16_t temperature_signed;
    double temp_final;
    int16_t temp_lsb;
    int16_t temp_msb;

    uint8_t tem = 0x22; // Address of Temperature Sensor data (section 5.3.7)
    uint8_t temp[2];

    /*ASSUMES THAT THE TEMPERATURE SENSOR SHARES THE ADDRESS OF THE ACCELEROMETER*/
        
    i2c_write_blocking(I2C_PORT, addr_accel, &tem, 1, true);    
    i2c_read_blocking(I2C_PORT, addr_accel, temp, 2, false);
        

    // Following section of code comes from the conversion information in section 5.3.7 of the BMI088 Datasheet 
    //temperature_unsigned = (temp[1]*8) + (temp[0] / 32);
    temperature_unsigned = (temp[0]*8) + (temp[1] / 32);
    if (temperature_unsigned > 1023){
        temperature_signed = temperature_unsigned - 2048;
    }else{
        temperature_signed = temperature_unsigned;
    }

    float degrees_per_LSB = 0.125;
    float temp_offset = 23;

    temp_final = 100* ((float)temperature_signed * degrees_per_LSB) + temp_offset;
    bmi088_temp = (int16_t) temp_final;

    if (temp[0] == 0x80){
        //printf("Invalid Measurement \n");
    }else{
        //printf("Temperature in C: %.2f\n", temp_final);
    } 
    //sleep_ms(1000);

}

void BMI088_get_accel() {
    i2c_write_blocking(I2C_PORT, addr_accel, &accel_val, 1, true);
    i2c_read_blocking(I2C_PORT, addr_accel, imu_accel, 6, false);

    x_accel = imu_accel[1]*256 + imu_accel[0];
    y_accel = imu_accel[3]*256 + imu_accel[2];
    z_accel = imu_accel[5]*256 + imu_accel[4];

    // Data conversion formula from section 5.3.4 of Datasheet
    X_accel = ((float)x_accel / 32768) * 1000 * pow(2, (ACC_RANGE + 1)) * 1.5;   // 0x01 is the range setting for the accelerometer
    Y_accel = ((float)y_accel / 32768) * 1000 * pow(2, (ACC_RANGE + 1)) * 1.5;  // offset to make the accelerations correct 
    Z_accel = ((float)z_accel / 32768)* 1000 * pow(2, (ACC_RANGE + 1)) * 1.5;   // after test, accel due to gravity is only .66g, 1.5 gets it to 1g
    
    // convert to int16_t for memoery storage
    Xaccel = (int16_t) X_accel;
    Yaccel = (int16_t) Y_accel;
    Zaccel = (int16_t) Z_accel;
    // Print to serial monitor
    //printf("Acceleration in mg\n");
    //printf("X: %ld\n", x_accel);
    //printf("Y: %ld\n", y_accel);
    //printf("Z: %ld\n", z_accel);

    //printf("X: %.2f | Y: %.2f | Z: %.2f\r\n", X_accel, Y_accel, Z_accel);

    //printf("FLOAT:%.2f\n", X_accel);
    //printf("%d", Xaccel)
    //sleep_ms(100); 
    
}

void BMI088_get_gyro() {
    i2c_write_blocking(I2C_PORT, addr_gyro, &gyro_val, 1, true);
    i2c_read_blocking(I2C_PORT, addr_gyro, imu_gyro, 6, false);

    // Alternate MSB,LSB conversion to real values
    // accelX = (int16_t)((imu_gyro[1]<<8) | imu_gyro[0]);
    // accelY = (int16_t)((imu_gyro[3]<<8) | imu_gyro[2]);
    // accelZ = (int16_t)((imu_gyro[5]<<8) | imu_gyro[4]);
    x_gyro = imu_gyro[1]*256 + imu_gyro[0];
    y_gyro = imu_gyro[3]*256 + imu_gyro[2];
    z_gyro = imu_gyro[5]*256 + imu_gyro[4];


    //Comes from Datasheet, Barometer  data conversion, section 5.3.4 
    // converts to degrees per sec
    X_gyro = ((float)x_gyro / 32768) * 1000 * pow(2, (GYRO_RANGE + 1));   // 0x00 is the range setting for the gyroscope
    Y_gyro = ((float)y_gyro / 32768) * 1000 * pow(2, (GYRO_RANGE + 1));
    Z_gyro = ((float)z_gyro / 32768) * 1000 * pow(2, (GYRO_RANGE + 1));

    Xgyro = (int16_t) X_gyro;
    Ygyro = (int16_t) Y_gyro;
    Zgyro = (int16_t) Z_gyro;

    // when OBC is oriented face up 
    //printf("Pitch: %.2f | Roll: %.2f | Yaw: %.2f\r\n", X_gyro, Y_gyro, Z_gyro);
    /*TEST THIS NEXT TIME */
    // float accel_x_in_g = X- / 1000;
    // float accel_y_in_g = Y / 1000;
    // float accel_z_in_g = G / 1000;
    // printf("Acceleration in G\n")
    // printf("X: %.2f | Y: %.2f | Z: %.2f\r\n", accel_x_in_g, accel_x_in_g, accel_z_in_g);
    // sleep_ms(300); 

    // Print to serial monitor
    //printf("Angular Velocity in m Degrees\n");
    //printf("X: %.2f | Y: %.2f | Z: %.2f\r\n", X_gyro, Y_gyro, Z_gyro);
    
}
