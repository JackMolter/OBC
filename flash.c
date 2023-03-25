// code to get the serial ID of a flash chips on the OBC
// code also reads and writes data from flash over SPI
// DMA code doesnt work copied from pico_examples
// TODO: make some DMA code that works 

// see flash file in drive for more info 

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/spi.h"

#include "flash.h"

static uint cs_pin; // to drive high or low 
static uint dma_chan;

uint8_t page_buf[FLASH_PAGE_SIZE]; // length of data
const uint32_t target_addr = 0; // address of the first data that is to be read

void flash_init(uint cs){
    cs_pin = cs;
    
    // set up
    spi_init(FLASH_SPI, FLASH_BAUD);
    gpio_set_function(FLASH_MISO,GPIO_FUNC_SPI);
    gpio_set_function(FLASH_MOSI,GPIO_FUNC_SPI);
    gpio_set_function(FLASH_SCK,GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(cs_pin);
    gpio_put(cs_pin, 1);
    gpio_set_dir(cs_pin, GPIO_OUT);
}

void flash_write_enable() {
    gpio_put(FLASH_CS, 0); // pull cs pin low to start transmission
    uint8_t buf[1];

    buf[0] = WRITE_ENABLE;
    spi_write_blocking(FLASH_SPI, buf, 1);

    gpio_put(FLASH_CS, 1); // pull cs pin high to end transmission
}

// used to determine things like if the chip is busy or completed a read/write
//      can be used at any time 
// can be modified to get the 2nd or 3rd status registers
void flash_read_register() {
    gpio_put(FLASH_CS, 0); // pull cs pin low to start transmission
    uint8_t buf[1];
    uint8_t reg[8];

    buf[0] = READ_REGISTER3;
    spi_write_blocking(FLASH_SPI, buf, 1);
    spi_read_blocking(FLASH_SPI, 0, reg, 8);

    gpio_put(FLASH_CS, 1); // pull cs pin high to end transmission
}

void flash_read_data(uint32_t addr, uint8_t *buf, size_t len) {
    static uint8_t read_dat[3];
    read_dat[0] = READ_DATA; // command to read
    read_dat[1] = addr >> 16;
    read_dat[2] = addr >> 8;
    read_dat[3] = addr; // set to 0 if reading a full page 

    gpio_put(FLASH_CS, 0); // pull cs pin low to start transmission
    spi_write_blocking(FLASH_SPI, read_dat, 4);
    spi_read_blocking(FLASH_SPI, 0, buf, len);

    gpio_put(FLASH_CS, 1); // pull cs pin high to end transmission
    sleep_ms(1);
}

// almost the same as read data with some extra stuff
void flash_write_data(uint32_t addr, uint8_t *buf, size_t len) {
    static uint8_t write_dat[3];

    write_dat[0] = WRITE_DATA; // command to write
    write_dat[1] = addr >> 16;
    write_dat[2] = addr >> 8;
    write_dat[3] = addr;

    flash_write_enable(); // enable write 
    gpio_put(FLASH_CS, 0); // pull cs pin low to start transmission
    spi_write_blocking(FLASH_SPI, write_dat, 4);
    spi_write_blocking(FLASH_SPI, buf, len);
    gpio_put(FLASH_CS, 1); // pull cs pin high to end transmission
    sleep_ms(1);    // max time to execute program 

}


void get_id() {
    uint8_t ID_BUF[5];
    uint8_t SixFour_ID[64];

    ID_BUF[0] = READ_ID;
    ID_BUF[1] = DUMMY_BYTE;
    ID_BUF[2] = DUMMY_BYTE;
    ID_BUF[3] = DUMMY_BYTE;
    ID_BUF[4] = DUMMY_BYTE;

    gpio_put(FLASH_CS, 0); // pull cs pin low to start transmission
    spi_write_blocking(FLASH_SPI, ID_BUF, 5);
    spi_read_blocking(FLASH_SPI, 0 ,SixFour_ID, 64);
    gpio_put(FLASH_CS, 1); // pull cs pin high to end transmission

    // print to serial the unique ID
    for ( int i = 0; i < 64; i++) {
        printf("%02x", SixFour_ID[i]);
    }
}
void flash_erase_4k(uint32_t addr) {    // sector erase
    static uint8_t erase_dat4[3];
    erase_dat4[0] = ERASE_4; 
    erase_dat4[1] = addr >> 16;
    erase_dat4[2] = addr >> 8;
    erase_dat4[3] = addr;

    flash_write_enable();
    gpio_put(FLASH_CS, 0);
    spi_write_blocking(FLASH_SPI, erase_dat4, 4);
    gpio_put(FLASH_CS, 1);
    sleep_ms(400);  // 45 ms nominal, max is 400ms
}
void flash_erase_32k(uint32_t addr) {
    static uint8_t erase_dat32[3];
    erase_dat32[0] = ERASE_32; 
    erase_dat32[1] = addr >> 16;
    erase_dat32[2] = addr >> 8;
    erase_dat32[3] = addr;

    flash_write_enable();
    gpio_put(FLASH_CS, 0);
    spi_write_blocking(FLASH_SPI, erase_dat32, 4);
    gpio_put(FLASH_CS, 1);
    sleep_ms(1600);
}
void flash_erase_64k(uint32_t addr) {
    static uint8_t erase_dat64[3];
    erase_dat64[0] = ERASE_64; 
    erase_dat64[1] = addr >> 16;
    erase_dat64[2] = addr >> 8;
    erase_dat64[3] = addr;
    
    flash_write_enable();
    gpio_put(FLASH_CS, 0);
    spi_write_blocking(FLASH_SPI, erase_dat64, 4);
    gpio_put(FLASH_CS, 1);
    sleep_ms(2000);
}
void flash_erase_chip() {
    static uint8_t erase_chip[1];
    erase_chip[0] = ERASE_CHIP; 

    flash_write_enable();
    gpio_put(FLASH_CS, 0);
    spi_write_blocking(FLASH_SPI, erase_chip, 1);
    gpio_put(FLASH_CS, 1);
    sleep_ms(1000 * 200);   // max time 200 seconds
}
int flash_busy() {
    //dma_channel_is_busy(dma_chan)
}

// probably useless, but could maybe have some purpose later 
void read_chip(uint16_t starting_addr, uint8_t *buf){
    static uint8_t read_dat[3];
    size_t page_len = 256;

    read_dat[0] = READ_DATA; // command to read
    read_dat[1] = starting_addr >> 16;
    read_dat[2] = starting_addr >> 8;
    read_dat[3] = starting_addr; // set to 0 if reading a full page 

    // while loop will read every single page and put it in the variable you choose
    while (starting_addr < 0xFFFFFF) { 
        gpio_put(FLASH_CS, 0); // pull cs pin low to start transmission
        spi_write_blocking(FLASH_SPI, read_dat, 4);
        spi_read_blocking(FLASH_SPI, 0, buf, page_len);

        gpio_put(FLASH_CS, 1); // pull cs pin high to end transmission
        sleep_ms(1);
    }
}


// dummy data for test
// best practice is to start with largest datatype first
// If no value is given to a variable garbage values will be put in its place
/*
typedef struct FlightData_{
    int64_t pressure;
    int32_t latitude;
    int32_t longitude;
    uint16_t temp1;  // -40-70 C
    int16_t accelX1;
    uint8_t status;

} FlightData;
*/

// reads first page of flash, changes flash, reads flash again, then erases flash
/*
int main() { `
    stdio_init_all();

    // struct data for test
    struct FlightData_ FlightData;       // declare FlightData as a struct  
    FlightData.pressure = 0x17AE8;       // mbar 97000
    FlightData.latitude = 0x5F53A;       // 390458, will need to be converted using 10e-4
    FlightData.longitude = 0xBB1CD;      // 766413, will need to be converted using 10e-4
    FlightData.temp1 = 0x834;            // C 2100, needs 10e-2 convertion
    FlightData.accelX1 = 0x4B0;          // microgravities 1200 = 1.2g
    FlightData.status = 0x01;            // could signify anything we want

    sleep_ms(2000);     // if sleep isnt added after this RP2040 struggles to connect over USB ?? weird
    printf("SPI flash example\n");
    setup(FLASH_CS);
    sleep_ms(100);
    flash_read_data(target_addr, page_buf, FLASH_PAGE_SIZE); // read data of all 0s

    int i = 0;
    uint16_t tt_addr = 0;

    // loop will fill data on first page and second page 
    while (i < 13) {
        flash_write_data(tt_addr, &FlightData,32); // write some data 21 is length of FlightData
        tt_addr = tt_addr + 32;
        i++;

    }
    sleep_ms(1000);
    flash_read_data(target_addr, page_buf, FLASH_PAGE_SIZE); // read the new data
    flash_read_data(target_addr + 256, page_buf, FLASH_PAGE_SIZE); // read the new data, next page 

    sleep_ms(10);
    flash_erase_4k(target_addr);    //erase the data
    flash_read_data(target_addr, page_buf, FLASH_PAGE_SIZE); // read the new data
}

// IGNORE ALL
/*
void dma_do_stuff(){ // doesn't work
    sleep_ms(8000); // minicom time 

    // writes data to spi and then reads it back 
    #define TEST_SIZE 1024
    // Grab some unused dma channels
    const uint dma_tx = dma_claim_unused_channel(true);
    const uint dma_rx = dma_claim_unused_channel(true);
    static uint8_t txbuf[TEST_SIZE];
    static uint8_t rxbuf[TEST_SIZE];
    for (uint i = 0; i < TEST_SIZE; ++i) {
        txbuf[i] = i;
    }
    // Configure TX

    dma_channel_config c = dma_channel_get_default_config(dma_tx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_dreq(Flash_SPI, true));

    dma_channel_configure(dma_tx, &c,
                          &spi_get_hw(Flash_SPI)->dr, // write address
                          txbuf, // read address
                          TEST_SIZE, // element count (each element is of size transfer_data_size)
                          false); // don't start yet    

    // Configre RX (pretty simular to tx)
    c = dma_channel_get_default_config(dma_rx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_dreq(Flash_SPI, false));
    channel_config_set_read_increment(&c, false); // this sets the speed i believe
    channel_config_set_write_increment(&c, true);

    dma_channel_configure(dma_rx, &c,
                          rxbuf, // write address
                          &spi_get_hw(Flash_SPI)->dr, // read address
                          TEST_SIZE, // element count (each element is of size transfer_data_size)
                          false); // don't start yet

    // Begin data transfer
    printf("begin the trans\n");
    dma_start_channel_mask((1u << dma_tx) | (1u << dma_rx));
    dma_channel_wait_for_finish_blocking(dma_rx);   // wait for the rx to finish
    if (dma_channel_is_busy(dma_tx)) {
        panic("RX completed before TX");    // apparently panic is a real function in c
    }
    
    printf("All good\n");
    dma_channel_unclaim(dma_tx);
    dma_channel_unclaim(dma_rx);


}
*/