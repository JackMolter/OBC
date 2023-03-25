#include <stdio.h>
#include <math.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/spi.h"
#include "hardware/pwm.h"

#include "GPS.h"
#include "IMU.h"
#include "lwgps.h"
#include "barometer.h"
#include "flash.h"
#include "OpenLST.h"
#include "LED.h"

    // TODO: change both OpenLST settings so that reset time is changed to 12 hours from 10 minutes
    // this prevents many possible issues with the data sent over UART every reset 
    // ask Nathan about possible unintended consequences 

    // All GPS readings are surpressed for indoor testing

// initilize states
// see file for indepth state explanation
void standby();
void flight_view();
void flight();
void tree();



// data struct init
typedef struct FlightData_{
    int32_t pressure;
    uint16_t temp1;
    //int32_t latitude;
    //int32_t longitude;
    //int32_t altitude;
    //uint8_t sats;       // numbers of satellites 
    int16_t temp2;  // -40-70 C
    int16_t accelX;
    int16_t accelY;
    int16_t accelZ;
    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;
    //uint8_t status;
    //uint8_t gps_status;

} FlightData;
//uint8_t FlightData_length = 8 + 2 + 4 + 4 + 4 + 1 + 2 + (2 * 6) + 1 + 1; 
uint8_t FlightData_length = 20;
// init other stuff
lwgps_t hgps;

// init variables 
// This is not very efficient
int32_t* pressure;
int32_t* temperature;
int32_t P;
int32_t TEMP;
int16_t* Xaccel;
int16_t* Yaccel;
int16_t* Zaccel;
int16_t* Xgyro;
int16_t* Ygyro;
int16_t* Zgyro;
int16_t* bmi088_temp;
char con[1000];

// trigger and memory variables
flash_memory_addr = 0x000000;

int standby_trigger = 0;        // variable to set switch state
int flight_view_trigger = 0;    
int flight_trigger = 0;

int status_command = 0;         // variable to set status state
uint16_t flight_time = 0;       // time since last command recieved

void main() {
    // init uart/usb 
    stdio_init_all();

    standby();

    flight_view();
    printf("exit");

    flight();

    tree();

    

}

void standby() {
    uint16_t time = 0;  // variable to increase as time elapses 
    int flight_command = 0;
    int status_report = 0;
    // initilize all 
    BMI088_init();  //IMU
    gps_init();     // GPS
    ms5607_init();  // barometer
    ms5607_prom_read();
    flash_init(FLASH_CS);
    openlst_setup();
    //openlst_setup();
    sleep_ms(100);

    // resets
    ms5607_reset();
    sleep_ms(50);

    while(1) {


        fading_blue();  // fade an led for visual sign of state 

        if (status_report == 8) {   // send status every 20 seconds

            // send status to groundstation
            // TODO : create status things and test

            status_report = 0;  //reset
        }

        time++;
        status_report++;
        // trigger logic

        // check UART FIFO for a command to send status
        // if command is found send status
        flight_command = check_command();
        // if time runs out 
        if (time > 400) {       // currently set to wait 20 minutes then enter flight ?? Idk how long to make this fail safe state change 
            standby_trigger = 1;
        }

        // command to enter flight_view 
        if (flight_command == 1) {
            standby_trigger = 1;
        }

        switch (standby_trigger) {
            case 0:
                // remain in standby state
                printf("Standby\n");
                break;

            case 1:
                // enter flight_view
                //pwm_set_gpio_level(LEDB, 255 * 255);    // turns blue led off 
                printf("Entering flight view state");
                return;

            default:
                break;


        }
    }

}

void flight_view() {

    // get all data readings
    // currently this gets data every 8 seconds 
    // 4 seconds of that are the LED blink
    // .5 second is added sleep
    // minimum time to execute ~ 3 seconds 
    while(1) { 
        led_blink(1);
        // get data from all sensors 
        ms5607_get_pressure();  // pressure and temp
        BMI088_get_temp();      // temp 
        BMI088_get_accel();     // translational acceleration
        BMI088_get_gyro();      // rotational acceleration
        gps_read_data();        // LLA, # of sats, time, speed, GPS valid status
        lwgps_init(&hgps);  // lwgps
        lwgps_process(&hgps, &con, strlen(con));
        
                // TODO: add voltage info from INA232, and add some health status stuff

        // change any float values to int
        hgps.latitude = hgps.latitude * 1000 * 1000;
        hgps.longitude = hgps.longitude * 1000 * 1000;
        hgps.altitude = hgps.altitude * 10;
        hgps.speed = hgps.speed * 1000;

        // store all the data in FLightData struct 
        struct FlightData_ FlightData;
        FlightData.pressure = pressure;
        FlightData.temp1 = temperature;
        FlightData.temp2 = TEMP;
        //FlightData.latitude = (int32_t) hgps.latitude;
        //FlightData.longitude = (int32_t) hgps.longitude;
        //FlightData.altitude = (int32_t) hgps.altitude;
        FlightData.accelX = Xaccel;
        FlightData.accelY = Yaccel;        
        FlightData.accelZ = Zaccel;
        FlightData.gyroX = Xgyro;
        FlightData.gyroY = Ygyro;
        FlightData.gyroZ = Zgyro;
        //FlightData.sats = hgps.sats_in_view;
        //FlightData.gps_status = hgps.is_valid;

        // ADDING THIS TO STRUCT BREAKS CODE, NEED TO FIND OUT WHY 
        //FlightData.status = 99;  // TODO: figure out what status stuff we want to report 

        // Store all data in flash memory
        flash_write_data(flash_memory_addr, &FlightData, FlightData_length);
        flash_memory_addr = flash_memory_addr + 32;

        // Downlink data to groundstation

            // TODO: add logic to defend against overwrite caused by powerdown address reset

        openlst_send_packet(&FlightData, FlightData_length, GROUNDSTATION_OPENLST_HWID);
        sleep_ms(500);  // set this sleep to the desired data collection frequency (0 sleep ~= 1 collect every 3 seconds)

        //printf("lat: %ld\n", FlightData.latitude);
        
        //printf("X Acceleration: %ld (mg) | Y Acceleration: %ld (mg)| Z Acceleration: %ld (mg) \n", Xaccel, Yaccel, Zaccel);
        //printf("X Rotation: %ld (deg/s) | Y Rotation: %ld (deg/s)| Z Rotation: %ld (deg/s) \n", Xgyro, Ygyro, Zgyro);    
        //printf("Pressure: %d (millibar)\n", pressure);
        //printf("Temp 1: %ld | Temp 2: %ld\n", temperature, bmi088_temp);
        /* printf("Latitude: %f degrees | Longitude: %f degrees | Altitude: %f (m)\n", hgps.latitude, hgps.longitude, hgps.altitude);
        printf("Sats in view: %d \r\n", hgps.sats_in_view);
        printf("CubeSat speed: %f (meters/s)\r\n", hgps.speed);
        printf("Time in UTC: %d (sec) | Launch Date: %d \n", hgps.seconds, hgps.date);
        //printf("Launch Data: %d (meters/s)", hgps.date);
        printf("GPS Valid status: %d\r\n", hgps.is_valid);
        */
        printf("\n");
        printf("\n");
        

        //printf("GPS_data: %s\n", con[0]);
        //printf("X Acceleration: %ld (mg)\n", Xaccel);
        //printf("Y Acceleration: %ld (mg)\n", Yaccel);
        //printf("Z Acceleration: %ld (mg)\n", Zaccel);
        //printf("X Rotation: %ld (deg/s)\n", Xgyro);
        //printf("Y Rotation: %ld (deg/s)\n", Ygyro);
        //printf("Z Rotation: %ld (deg/s)\n", Zgyro);
        //printf("Temp 2: %ld\n", bmi088_temp);
        //printf("Valid status: %d\r\n", hgps.is_valid);
        //printf("Latitude: %f degrees\r\n", hgps.latitude);
        //printf("Longitude: %f degrees\r\n", hgps.longitude);
        //printf("Altitude: %f meters\r\n", hgps.altitude);
        //printf("Sats in view: %d \r\n", hgps.sats_in_view);
        //printf("CubeSat speed: %f meters/s\r\n", hgps.speed);

            // TODO:
            // check UART FIFO for command from station to continue sending data
            // if command recieved, reset flight_time variable 



        flight_time++;
        
        // trigger logic 

        if (flight_time > 120) {     // ~5 mins 
            flight_view_trigger = 1;
        }

        switch(flight_view_trigger) {
            case 0:
                printf("In flight view\n");
                break;
            
            case 1:
                printf(" Entering flight state\n");
                return;

            default:
                break;



        }

    }
}

void flight() {
    uint16_t Flight_time = 0;
    printf("Entering Flight state");

    while(1) { 

        led_blink(3);     // erase after testing

        // get data from all sensors 
        ms5607_get_pressure();  // pressure and temp
        BMI088_get_temp();      // temp 
        BMI088_get_accel();     // translational acceleration
        BMI088_get_gyro();      // rotational acceleration
        gps_read_data();        // LLA, # of sats, time, speed, GPS valid status
        lwgps_init(&hgps);  // lwgps
        lwgps_process(&hgps, &con, strlen(con));
        
                // TODO: add voltage info from INA232, and add some health status stuff

        // change any float values to int
        hgps.latitude = hgps.latitude * 1000 * 1000;
        hgps.longitude = hgps.longitude * 1000 * 1000;
        hgps.altitude = hgps.altitude * 10;
        hgps.speed = hgps.speed * 1000;

        // store all the data in FLightData struct 
        struct FlightData_ FlightData;
        FlightData.pressure = pressure;
        FlightData.temp1 = temperature;
        FlightData.temp2 = TEMP;
        //FlightData.latitude = (int32_t) hgps.latitude;
        //FlightData.longitude = (int32_t) hgps.longitude;
        //FlightData.altitude = (int32_t) hgps.altitude;
        FlightData.accelX = Xaccel;
        FlightData.accelY = Yaccel;        
        FlightData.accelZ = Zaccel;
        FlightData.gyroX = Xgyro;
        FlightData.gyroY = Ygyro;
        FlightData.gyroZ = Zgyro;
        //FlightData.sats = hgps.sats_in_view;
        //FlightData.gps_status = hgps.is_valid;

        // ADDING THIS TO STRUCT BREAKS CODE, NEED TO FIND OUT WHY 
        //FlightData.status = 99;  // TODO: figure out what status stuff we want to report 

        // Store all data in flash memory
        flash_write_data(flash_memory_addr, &FlightData, FlightData_length);
        flash_memory_addr = flash_memory_addr + 32;

        // this sleep will determine data gathering rate 
        sleep_ms(3000); // this + ~2.5 seconds is the freq

        // logic to enter tree state

        // time
        if (Flight_time >= 1440) { //  guessing a 2 hour flight time (7200 / 5)
            flight_trigger = 1;
        }


            // TODO: add voltage cutoff logic from INA232
        

            switch(flight_trigger) {
        case 0:
            printf("In flight \n");
            break;

        case 1:
            printf(" Entering tree state\n");
            return;

        default:                    
            break;


        }
        

    }

}

void tree() {

}