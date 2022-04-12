#ifndef MESSAGE_SLOT_H
#define MESSAGE_SLOT_H

#include <errno.h>
#include <linux/ioctl.h>

// The major device number.
// We don't rely on dynamic registration
// any more. We want ioctls to know this
// number at compile time.
#define MAJOR_NUM 235

// define the ioctl command id for changing the channel
#define MSG_SLOT_CHANNEL 1433

// Set the file descriptor of the device driver == change message slot
#define IOCTL_SET_SLOT _IOW(MAJOR_NUM, 0, long)

#define DEVICE_RANGE_NAME "char_dev"
#define BUF_LEN 80
#define DEVICE_FILE_NAME "message_slot"
#define SUCCESS 0









#endif
