#include "message_slot.h"

#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int main(int args, char* argv[]) {
	if (args != 4) {
		errno = EINVAL;
		perror("Incorrect amount of arguments");
		exit(1);
	}
	
	// parsing the data
	char* chr_dev_path = argv[1]);
	int channel_id = atoi(argv[2]);
	
	// removing the terminating NULL character from the end of the string
	unsigned int message_length = strlen(argv[3]);
	char clean_message[message_length];
	memcpy(clean_message, argv[3], message_length);
	
	// open the device file
	int fd = open(chr_dev_path, O_WRONLY);
	
	// set the channel
	ioctl(fd, MSG_SLOT_CHANNEL, channel_id);
	
	// write the clean message into the channel buffer
	write(fd, clean_message, message_length);
	
	// close the device file
	close(fd);
}
