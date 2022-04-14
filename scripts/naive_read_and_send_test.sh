#!/bin/bash

# fetch the message from CMD
message=$1

# providing the interface for communication
sudo bash ../scripts/relink_module.sh # relinking module
sudo rm /dev/slot1 # removing the message slot
sudo mknod /dev/slot1 c 235 1 # recreating the message slot 

# recopmiling the reader and sender programs
{
	bash ../scripts/comp.sh
} &> /dev/null

# run the sender program
sudo ../sender /dev/slot1 1 $message # running the sender program

# read the sent message
read_message=$(sudo ../reader /dev/slot1 1)

# print the output of the aforetyped operations
echo -e "\n\e[4;34m\e[1;34mRESULTS\e[0m"
echo -e "The message sent is: \e[4;33m\e[1;33m${message}\e[0m\nIts length is: \e[4;33m\e[1;33m${#message}\e[0m"
echo -e "\nThe message read is: \e[4;33m\e[1;33m${read_message}\e[0m\nIts length is: \e[4;33m\e[1;33m${#read_message}\e[0m"

# verify that the read message is the previously sent message
if [[ "${read_message}" != "${message}" && ${#message} -le 128 ]]; then
	echo -e "\e[1;31mFAILED\e[0m" # print out a 'failed' message
	exit 1
fi


# print out a 'success' message
echo -e '\nFinally: \033[1;32mSUCCESS\e[0m'
exit 0
