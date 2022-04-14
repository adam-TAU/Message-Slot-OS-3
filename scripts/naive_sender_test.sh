#!/bin/bash

# providing the interface for communication
sudo bash ../scripts/relink_module.sh # relinking module
sudo rm /dev/slot1 # removing the message slot
sudo mknod /dev/slot1 c 235 1 # recreating the message slot 

# recompiling the reader and sender program
{
bash ../scripts/comp.sh
} &> /dev/null

# running the sender program
sudo ../sender /dev/slot1 1 hello

# showing the log for the sender program
dmesg
