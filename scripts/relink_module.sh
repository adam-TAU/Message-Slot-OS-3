#!/bin/bash

# cleaning up
{
	bash ../scripts/cleanup.sh
} &> /dev/null # clearing module files

{
	sudo dmesg -c
} &> /dev/null # clearing log

# creating the new module
make

# removing the recent module
sudo rmmod message_slot

# inserting the new module
sudo insmod message_slot.ko 235

# showing the log of the module creation
dmesg

# cleaning up
{
	bash ../scripts/cleanup.sh
} &> /dev/null # clearing module files


echo "Relinked the message_slot module"

