
// values
#define GPS_BAUD 9600 // for test
#define GPS_UART_ID uart1 

// pins 
#define GPS_UART_TX 4   //UART1 of OBC 
#define GPS_UART_RX 5

// functions

// initialize 
void gps_init();

// get GPS reading 
void gps_read_data();
