// This file will hold any commands we want to send from the ground station to the OpenLST
// This file could probably be combined with UART.c and renamed OpenLST
// All of this assumes that addind the header bytes and hwid to a set of data tells the OpenLST that it should
// transmitted. TODO: confirm this 

// to test: when a packet is sent over OpenLST are the header and ending bytes included in the UART RX packet??
// example does 0x22 0x69 0x03 0x01 0x02 0x03 0x01 0x17 --> 0x01 0x02 0x03 ???? 

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#include "settings.h"
#include "commands.h"
#include "OpenLST.h"

uint8_t com[9];

// start here then improve upon
// assumes that FIFO has been enabled, and some data has been found in the TX fifo
// IDEA: make all commands the same length so that stuff is easier 
int check_command() {
    uart_set_fifo_enabled(OPENLST_UART_ID, 1);
    uint8_t buf[8]; // rn commands are length 9
    buf[7] = 12;    // 
    int i = 0;

    
    // *************************
    // IF OPENLST LOSES POWER THIS GETS STUCK HERE FOREVER
    // *************************
    while(uart_is_readable(OPENLST_UART_ID) != 0) { // if there exists data in the fifo
        uart_read_blocking(OPENLST_UART_ID, com, 9);    // read the data in fifo
        printf("reading\n");
    }

    printf("9: %d\n", com[8]);
    printf("");
    // logic to determine which command has been sent 
    
    // enter flight_view state 
    // com8 is imu[5] which is the MSB of the Z acceleration
    // if the OBC is placed vertical to the ground with USB side down for a few seconds
    // the OBC enters flight mode
    if (com[8] == 1) {
        return 1;
    }
    else {
        return 0;
    }

    

}

