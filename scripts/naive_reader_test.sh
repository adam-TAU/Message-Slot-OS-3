#!/bin/bash

# providing the interface for communication
sudo bash ../scripts/relink_module.sh # relinking module
bash ../scripts/comp.sh # recompiling the reader and sender programs

# running the reader program
sudo ../reader /dev/slot1 1

# showing the log of the reader program
dmesg
