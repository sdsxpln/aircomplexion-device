#!/bin/bash
# this is to get the testing program compiled to the location. All the libraries are tested before they are inducted in the main program
# author        : kneerunjun@gmail.com
# purpose       : only to get the progam compiled and place the executable in the right spot
gcc -c ./mq7/mq7.c -I ./mq7 -I ./adc -o./bin/mq7.o
# now we go ahead to compile
gcc testing.c -o ./bin/testing ./bin/mq7.o -lwiringPi -lwiringPiDev  -lm -lpthread -L ./bin -lco2
sudo ./bin/testing
