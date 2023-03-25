#include <stdint.h>
// values 
#define UART_BAUD_RATE 115200
#define OPENLST_UART_BAUD_RATE 115200
#define OPENLST_UART_ID uart0   // from the OpenLST schematic
#define GROUNDSTATION_OPENLST_HWID 0x0001
#define MAX_PAYLOAD_LEN 242     // from some testing ? 

#define OPENLST_PACKET_HEADER_1 0x22
#define OPENLST_PACKET_HEADER_2 0x69

// pins
#define OPENLST_UART_TX 16  // for OBC UART0
#define OPENLST_UART_RX 17


// functions 

// initilization
void openlst_setup();

// tests sending something
void openlst_test();
void openlst_ack_test();

// get telem command 
void openlst_get_telem(); 

// send a packet 
void openlst_send_packet(uint8_t *data, int len, int dest_hwid);

