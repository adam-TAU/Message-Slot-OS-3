#include "message_slot.h"

#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void print_err(char* error_message, bool terminate) {
	int tmp_errno = errno;
	perror(error_message); // this basically prints error_message, with <strerror(errno)> appended to it */
	errno = tmp_errno;
	
	if (terminate) {
		exit(1);
	}
}


int main(int args, char* argv[]) {
	if (args != 4) {
		errno = EINVAL;
		print_err("Incorrect amount of arguments", true);
	}
	
	// parsing the data
	char* chr_dev_path = argv[1];
	int channel_id = atoi(argv[2]);
	
	// removing the terminating NULL character from the end of the string
	unsigned int message_length = strlen(argv[3]);
	char clean_message[message_length];
	memcpy(clean_message, argv[3], message_length);
	
	// open the device file
	int fd;
	if ( 0 > (fd = open(chr_dev_path, O_WRONLY)) ) {
		print_err("Error with opening the device file", true);
	}
	
	// set the channel
	if ( 0 != ioctl(fd, MSG_SLOT_CHANNEL, channel_id) ) {
		print_err("Error with setting the message channel", true);
	}
	
	// write the clean message into the channel buffer
	if ( 0 > write(fd, clean_message, message_length) ) {
		print_err("Error with writing the message into the channel buffer", true);
	}
	
	// close the device file
	if ( 0 != close(fd) ) {
		print_err("Error with closing the device file", true);
	}
	
	exit(0);
}
