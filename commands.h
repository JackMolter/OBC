
// functions 

// check for any commands from the ground station 
void get_command();

// starting simple
int check_command();


// commands

// the ultimate command that does everything 
#define OBC_CMD_MASTER 0x4A //random hex 

// sends back status stuff
#define OBC_CMD_STATUS 0x43

// gets altitude from OBC, proboably only use this for testing purposes 
#define OBC_CMD_GETALT 0x4D