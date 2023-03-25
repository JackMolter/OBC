/*
Link to BMI088 Datasheet
https://download.mikroe.com/documents/datasheets/BMI088_Datasheet.pdf



*/

// pins
#define I2C_PORT i2c0
#define BMI088_BAUD 115200 // ? from video example
#define BMI088_SDA 8
#define BMI088_SCL 9
#define addr_accel 0x19 //0x18 when pin pulled to ground, accelerometer address
#define addr_gyro 0x69  // address of the gyroscope= 0x69 gyroscope 

// values
#define ACC_CONF 0xA7
#define ACC_RANGE 0x01
#define GYRO_RANGE 0x00
#define ACC_SOFTRESET 0xB6
#define GYRO_SOFTRESET 0xB6
#define NORMAL_ODR 0xA7
#define ACCEL_SELF_TEST 0x6D

// commands

void BMI088_init();

void BMI088_get_accel();

void BMI088_get_gyro();

void BMI088_get_temp();

