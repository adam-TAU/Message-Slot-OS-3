rm sender
rm reader

gcc -O3 -Wall -std=c11 message_sender.c -o ../sender 
gcc -O3 -Wall -std=c11 message_reader.c -o ../reader

echo "Compiled the reader/sender programs"
