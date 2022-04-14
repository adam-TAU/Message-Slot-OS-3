#!/bin/bash


bash ../scripts/cleanup.sh # cleaning up
make # creating new module
sudo dmesg -c # clearing log
sudo rmmod message_slot # removing recent module
sudo insmod message_slot.ko 235 # inserting new module
dmesg

bash ../scripts/cleanup.sh


echo "Relinked the message_slot module"

