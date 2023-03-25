
// values 
#define MS5607_id 0b1110111 //111011Cx // check as this is determined by the CSB pin (CBS 0)
#define MS5607_BAUD 112800
#define MS5607_PORT i2c0 // check

// pins
#define MS5607_SDA 8    // i2c0 of OBC
#define MS5607_SCL 9

// commands
#define MS5607_CONVERT_D1 0b01001000
#define MS5607_CONVERT_D2 0x58
#define MS5607_CMD_RESET 0b00011110
#define MS5607_CMD_ADC_READ 0x00
#define MS5607_CMD_PROM_READ_DATA 0b10100000    // contains factory data
#define MS5607_CMD_PROM_READ_C1 0b10100010      // calibration coef values range from 0 - 65535
#define MS5607_CMD_PROM_READ_C2 0b10100100      // calibration coef
#define MS5607_CMD_PROM_READ_C3 0b10100110      // calibration coef
#define MS5607_CMD_PROM_READ_C4 0b10101000  
#define MS5607_CMD_PROM_READ_C5 0b10101010
#define MS5607_CMD_PROM_READ_C6 0b10101100
#define MS5607_CMD_PROM_READ_CRC 0b10101110      // crc

// functions 
// initilization
void ms5607_init();

// reset required before anything
void ms5607_reset();

// reads factory set calibration values
void ms5607_prom_read();

// converts values to real temp/pressure
void ms5607_convert();
void ms5607_D1_conversion();
void ms5607_D2_conversion();

//
void ms5607_get_pressure();

