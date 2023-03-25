
// pins 
#define LEDR 14
#define LEDG 15
#define LEDB 18


// fucntions 

// fades the LED to make a cool rainbow
// TODO: find out why red doesnt shine bright 
void led_fade();

// blink an LED once 
// each blink is 4 seconds TODO: maybe change to 2 or 3 sec
 
void led_blink(uint8_t color);

// flow 1 color
// each fade is 2.5 seconds 
void fading_blue();

