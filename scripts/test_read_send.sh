#!/bin/bash


# providing the interface for communication
sudo bash ../scripts/relink_module.sh # relinking module
sudo rm /dev/slot1 # removing the message slot
sudo mknod /dev/slot1 c 235 1 # recreating the message slot 

# recopmiling the reader and sender programs
{
	bash ../scripts/comp.sh
} &> /dev/null

# run the sender program
sudo ../sender /dev/slot1 1 hello # running the sender program

# read the sent message
read_message=$(sudo ../reader /dev/slot1 1)

# verify that the read message is the previously sent message
if [ "${read_message}" != "hello" ]; then
	echo "The message read isn\'t \'hello\': ${read_message}"
	# print out a 'success' message	
	exit 1
fi

# print out a 'failed' message

