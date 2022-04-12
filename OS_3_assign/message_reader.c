#include "message_slot.h"








int main(int args, char* argv[]) {
	if (args != 3) {
		errno = EINVAL
		perror("Incorrect amount of arguments");
		exit(1);
	}
}
