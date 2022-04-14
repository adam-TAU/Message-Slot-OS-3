#!/bin/bash

sudo bash ../scripts/relink_module.sh

bash ../scripts/comp.sh
sudo ../reader /dev/slot1 1
dmesg
