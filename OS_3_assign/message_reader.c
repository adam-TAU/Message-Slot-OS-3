#include "message_slot.h"

#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <stdio.h>
#include <stdlib.h>



static char the_message[BUF_LEN];
static int count;


int main(int args, char* argv[]) {
	if (args != 3) {
		errno = EINVAL;
		perror("Incorrect amount of arguments");
		exit(1);
	}
	
	// parsing the data
	char* chr_dev_path = argv[1]);
	int channel_id = atoi(argv[2]);
	
	// open the device file
	fd = open(chr_dev_path, O_RDONLY);
	
	// Read the message, and save the amount of bytes read into the variable 'count'
	char message[BUF_LEN];
	count = read(fd, message, BUF_LEN);
	
	// close the device file
	close(fd);
	
	// verify that the return value of the read syscall indicates success
	if (count < 0) {
		perror("Error with reading the message from the message channel");
		exit(1);
	}
	
	// Print the message
	write(STDOUT_FILENO, the_message, count);
}
