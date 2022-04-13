rm sender
rm reader

gcc -O3 -Wall -std=c11 ../OS_3_assign/message_sender.c -o ../sender 
gcc -O3 -Wall -std=c11 ../OS_3_assign/message_reader.c -o ../reader
