#ifndef MESSAGE_SLOT_H
#define MESSAGE_SLOT_H

#include <linux/ioctl.h>

// The major device number.
// We don't rely on dynamic registration
// any more. We want ioctls to know this
// number at compile time.
#define MAJOR_NUM 235

// define the ioctl command id for changing the channel
#define MSG_SLOT_CHANNEL _IOW(MAJOR_NUM, 0, unsigned long)

// simple boolean interface
#define true 1
#define false 0
#define bool int

// other macros
#define DEVICE_RANGE_NAME "char_dev"
#define BUF_LEN 128
#define DEVICE_FILE_NAME "message_slot"
#define SUCCESS 0

// a function that acceps an error message and prints the error message along with 
void print_err(char* error_message, bool terminate);


#endif
