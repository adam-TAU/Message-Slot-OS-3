

{
sudo rm /dev/test0
sudo rm /dev/test1
sudo rmmod message_slot
sudo insmod ../OS_3_assign/message_slot.ko
} 2>/dev/null



sudo mknod /dev/test0 c 235 0
sudo mknod /dev/test1 c 235 1
sudo chmod 0777 /dev/test0
sudo chmod 0777 /dev/test1




gcc -O3 -Wall -std=c11 ex3_tester.c -o tester
./tester
