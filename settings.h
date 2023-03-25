// OpenLST 
#define FLIGHT_OPENLST_HWID 0x0002 // changed from 1
#define OPENLST_UART_BAUD_RATE 115200
#define OPENLST_UART_ID uart0
//#define OPENLST_UART_TX 5
//#define OPENLST_UART_RX 4

#define OPENLST_PACKET_HEADER_1 0x22
#define OPENLST_PACKET_HEADER_2 0x69
#define MAX_PAYLOAD_LEN 242

// OpenLST UART
#define OPENLST_UART_TX 16  // for OBC --> OpenLST
#define OPENLST_LSTUART_RX 17



// Flash
#define FLASH_BAUD 1000 * 1000 // for test 
#define FLASH_PAGE_SIZE 256
#define FLASH_SECTOR_SIZE 4096
#define RANDOM_DATA_SIZE 256
#define FLASH_CS_DELAY 100 // us

#define FLASH_SPI spi0  //pins
#define FLASH_MOSI 3
#define FLASH_MISO 0
#define FLASH_SCK 2 
#define FLASH_CS 1

#define FLASH_DUMMY_BYTE 0x00
#define FLASH_READ_ID 0x4B
#define FLASH_WRITE_ENABLE 0x06
#define FLASH_READ_DATA 0x03
#define FLASH_WRITE_DATA 0x02
#define FLASH_ERASE_4 0x20
#define FLASH_ERASE_32 0x52
#define FLASH_ERASE_64 0xD8
#define FLASH_ERASE_CHIP 0xC7

#define FLASH_READ_REGISTER1 0x05 // probably the only useful one
#define FLASH_READ_REGISTER2 0x35
#define FLASH_READ_REGISTER3 0x15

// IMU
#define BMI088_ID 0x19 // 
#define ACCEL_DATA_LEN 6 
#define I2C_PORT i2c0
#define BMI088_BAUD 112800

#define BMI088_SDA 8
#define BMI088_SCL 9

#define ACC_SOFTRESET 0x7E  // reset
#define ACC_RESET 0xB6

#define ACC_PWR_CTRL 0x7D   // power on or off
#define ACC_ON 0x04
#define ACC_OFF 

#define ACC_RANGE 0x41  // set the max ranges 
#define ACC_RANGE_3G 0x00
#define ACC_RANGE_6G 0x01
#define ACC_RANGE_12G 0x02
#define ACC_RANGE_24G 0x03
 
#define GYRO_LMP1 0x11  // turn gyro on or suspend 
#define GYRO_NORMAL 0x00
#define GYRO_SUSPEND 0x80

#define GYRO_RANGE 0x0F     // range rate 
#define GYRO_RANGE_2000 0x00
#define GYRO_RANGE_1000 0x01
#define GYRO_RANGE_500 0x02
#define GYRO_RANGE_250 0x3
#define GYRO_RANGE_125 0x04

#define GYRO_BANDWIDTH 0x10     // Bandwidth and output data rate 
#define GYRO_BANDWIDTH_532 0x00
#define GYRO_BANDWIDTH_230 0x01
#define GYRO_BANDWIDTH_116 0x02
#define GYRO_BANDWIDTH_47 0x03
#define GYRO_BANDWIDTH_23 0x04
#define GYRO_BANDWIDTH_12 0x05  // ODR = 100
#define GYRO_BANDWIDTH_64 0x06  // ODR = 200
#define GYRO_BANDWIDTH_32 0x07  // ODR = 100

// LED
#define LEDR 14     //pins
#define LEDG 15
#define LEDB 18

// TODO: refine these a little, we got multiple UARTS and stuff 
