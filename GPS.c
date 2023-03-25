// code that gets GPS data from the OBC and prints out an lla 
// can easily be mofiied to print other info
// TODO: change this so lla and time data are flashed to storage

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

#include "pico/stdlib.h"

//#include "lwgps_opts.h"
#include "lwgps_opt.h"
#include "lwgps.h"
#include "GPS.h"

char con[1000]; //holds the gps data 1000 bytes will always hold enough for a valid reading 
lwgps_t hgps;   // lwgps 
int i = 0;  

/**
 * \brief           Dummy data from GPS receiver
 */
/*const char gps_rx_data[] = ""
                           "$GPRMC,183729,A,3907.356,N,12102.482,W,000.0,360.0,080301,015.5,E*6F\r\n"
                           "$GPRMB,A,,,,,,,,,,,,V*71\r\n"
                           "$GPGGA,183730,3907.356,N,12102.482,W,1,05,1.6,646.4,M,-24.1,M,,*75\r\n"
                           "$GPGSA,A,3,02,,,07,,09,24,26,,,,,1.6,1.6,1.0*3D\r\n"
                           "$GPGSV,2,1,08,02,43,088,38,04,42,145,00,05,11,291,00,07,60,043,35*71\r\n"
                           "$GPGSV,2,2,08,08,02,145,00,09,46,303,47,24,16,178,32,26,18,231,43*77\r\n"
                           "$PGRME,22.0,M,52.9,M,51.0,M*14\r\n"
                           "$GPGLL,3907.360,N,12102.481,W,183730,A*33\r\n"
                           "$PGRMZ,2062,f,3*2D\r\n"
                           "$PGRMM,WGS84*06\r\n"
                           "$GPBOD,,T,,M,,*47\r\n"
                           "$GPRTE,1,1,c,0*07\r\n"
                           "$GPRMC,183731,A,3907.482,N,12102.436,W,000.0,360.0,080301,015.5,E*67\r\n"
                           "$GPRMB,A,,,,,,,,,,,,V*71\r\n";
*/
void gps_init() {

    // UART setup 
    uart_init(GPS_UART_ID, GPS_BAUD);

    gpio_set_function(GPS_UART_TX, GPIO_FUNC_UART);     // sets pins
    gpio_set_function(GPS_UART_RX, GPIO_FUNC_UART);
    //uart_set_hw_flow(GPS_UART_ID, false, false);    // flow control
}

void gps_read_data() {
    uart_set_fifo_enabled(GPS_UART_ID, 1);

    //printf(" Begin data gathering\n");

    int i = 0;
    uint8_t buf[1];

    while(i < 1000) {

        // this is totally wrong but somehow works...
        // uart_is_readable function is always 0
        // not sure how or why this works but it does
        // TODO: test this and make it actually work
        if(uart_is_readable != 0) {
            uart_read_blocking(GPS_UART_ID, buf, 1);

            con[i] = buf[0];
            i++;
        }
        else {
            sleep_us(1);
        }
    }
    //printf("finally works :)");
}
/* code that makes it so data gathering starts at beginning of trasnmission, 
/ the lwgps library already does this but this took hours so ill keep it as a reminder 

int i = 0;
void gps_read_data(){
    uint8_t buf[1];
    uint8_t con[4];

    //uart_is_readable(GPS_UART_ID);
    uart_set_fifo_enabled(GPS_UART_ID, 1);

    uint8_t check = 0x00;   // this stays 0 until the data is in the correct order (timing) for collection 

    while(1) {
        
        if (uart_is_readable(GPS_UART_ID) != 0) {   // if data is in the FIFO
            uart_read_blocking(GPS_UART_ID, buf, 1);    // read the data 
            //uart_write_blocking(GPS_UART_ID, buf, 1);

            con[i] = buf[0];

            if( con[0] == 0x24 && con[1] == 0x47 && con[2] == 0x4E && con[3] == 0x52) {
               //for(int j = 0; j < 4; j ++) {
               // printf("%x\n", con[j]);
               //}
               check = 0x01;
            }
            if(i < 4) {
                i++;
            }
            else {
                i = 0;
            }

            if( check == 1) {
                uart_write_blocking(GPS_UART_ID, buf, 1);
                if(uart_is_readable == 0) {
                    // turn off data collection 
                    //exit();
                }
            }
        }
        else {
            (sleep_us(1)); // wait until some data appears 
        }
    }

    printf("hey it worked");

}

*/

/*
int main() {
    stdio_init_all();
    sleep_ms(120000);   // sleep for 2 minutes to allow for GPS fix 

    gps_init();
    sleep_ms(30);
    gps_read_data();
    
    lwgps_init(&hgps);  // initilize lwgps
    
    // Process all input data 
    lwgps_process(&hgps, con, strlen(con));

    // Print messages 
    printf("Valid status: %d\r\n", hgps.is_valid);
    printf("Latitude: %f degrees\r\n", hgps.latitude);
    printf("Longitude: %f degrees\r\n", hgps.longitude);
    printf("Altitude: %f meters\r\n", hgps.altitude);
    
    return 0;

}
*/