// values
#define FLASH_BAUD 115200 // for test 
#define FLASH_PAGE_SIZE 256
#define FLASH_SECTOR_SIZE 4096
#define RANDOM_DATA_SIZE 256
#define FLASH_CS_DELAY 100 // us
// __not_in_flash_func means a function wont be stored in flash 

// pins
#define FLASH_SPI spi0
#define FLASH_MOSI 3
#define FLASH_MISO 0 //4
#define FLASH_CS 1
#define FLASH_SCK 2 

// commands
#define DUMMY_BYTE 0x00
#define READ_ID 0x4B
#define WRITE_ENABLE 0x06
#define READ_DATA 0x03
#define WRITE_DATA 0x02 // page program, page 36 of datasheet
#define ERASE_4 0x20    // time critical, not enough time = incorrect erase
#define ERASE_32 0x52
#define ERASE_64 0xD8
#define ERASE_CHIP 0xC7 // not working for some reason 

#define READ_REGISTER1 0x05 // probably the only useful one
#define READ_REGISTER2 0x35
#define READ_REGISTER3 0x15



// initialize functions 
void __not_in_flash_func(flash_init)(uint cs);

// get the 64 bit chip id
void __not_in_flash_func(get_id)(); 

// write enable 
// need to run this or data can not be written to
// used as a fail safe for accidental writing 
void __not_in_flash_func(flash_write_enable)(); 

// read status registers
void __not_in_flash_func(flash_read_register)();

// read a single page of data
// needs address of data to read, length of data
void __not_in_flash_func(flash_read_data)(uint32_t addr, uint8_t *buf, size_t len);

// write data to a specified address
// length of data is critical
//addresses ending in 00 will write the entire page
// TODO: test this more, effects of partial writes
void __not_in_flash_func(flash_write_data)(uint32_t addr, uint8_t *buf, size_t len);

// 4k Erase
//  addr is start of 4k block address 
// Erases 4k-bytes of data, the erased state is all 1s.
void __not_in_flash_func(flash_erase_4k)(uint32_t addr);

// 32k erase
void __not_in_flash_func(flash_erase_32k)(uint32_t addr);

// 64k erase
void __not_in_flash_func(flash_erase_64k)(uint32_t addr);

// chip erase
void __not_in_flash_func(flash_erase_chip)();

// DMA stuff
// check if busy
int __not_in_flash_func(check_busy)();

static inline void __not_in_flash_func(dma_done_handle)();

int64_t __not_in_flash_func(cs_alarm)();

void __not_in_flash_func(dma_do_stuff)();

// read the entire chip starting from a designated address
// starting_addr is first address tor ead
// buf is where the data gets stored
void __not_in_flash_func(read_chip)(uint16_t starting_addr, uint8_t *buf);
