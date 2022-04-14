#include "message_slot.h"

#include <errno.h>	/* errors */
#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <stdio.h>
#include <stdlib.h>


void print_err(char* error_message, bool terminate) {
	int tmp_errno = errno;
	perror(error_message); // this basically prints error_message, with <strerror(errno)> appended to it */
	errno = tmp_errno;
	
	if (terminate) {
		exit(1);
	}
}



static char the_message[BUF_LEN];
static int bytes_read;


int main(int args, char* argv[]) {
	if (args != 3) {
		errno = EINVAL;
		print_err("Incorrect amount of arguments", true);
	}
	
	// parsing the data
	char* chr_dev_path = argv[1];
	int channel_id = atoi(argv[2]);
	
	// open the device file
	int fd;
	if ( 0 > (fd = open(chr_dev_path, O_RDONLY)) ) {
		print_err("Error with opening the device file", true);
	}
	
	// set the channel
	if ( 0 != ioctl(fd, MSG_SLOT_CHANNEL, channel_id) ) {
		print_err("Error with setting the message channel", true);
	}
	
	// Read the message and save the amount of bytes read into <bytes_read>
	if ( 0 > (bytes_read = read(fd, the_message, BUF_LEN)) ) {
		print_err("Error with reading bytes from message channel", true);
	}
	
	// close the device file
	if ( 0 != close(fd) ) {
		print_err("Error with closing the device file", true);
	}
	
	// Print the message
	if ( 0 > write(STDOUT_FILENO, the_message, bytes_read) ) {
		print_err("Error with printing the message to stdout", true);
	}
	
	exit(0);
}
