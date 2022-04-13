#!/bin/bash

sudo bash ../scripts/relink_module.sh
sudo rm /dev/slot1
sudo mknod /dev/slot1 c 235 1

bash ../scripts/comp.sh
sudo ../reader /dev/slot1 1
dmesg
