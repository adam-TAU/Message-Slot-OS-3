#include "message_slot.h"

#include <stdio.h>
#include <unistd.h>



static char the_message[BUF_LEN];
static int count;


int main(int args, char* argv[]) {
	if (args != 3) {
		errno = EINVAL;
		perror("Incorrect amount of arguments");
		exit(1);
	}
	
	/* Open the device file descriptor */
	fd = open();
	
	/* Read the message, and save the amount of bytes read into the variable 'count' */
	count = read();
	if (count < 0) {
		perror("Error with reading the message from the message channel");
		exit(1);
	}
	
	/* Print the message */
	write(STDOUT_FILENO, the_message, count);
}
