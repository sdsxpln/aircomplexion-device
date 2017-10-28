#!/bin/bash
# this is to get the c programme compliled to the location
# author        : kneerunjun@gmail.com
# purpose       : only to get the progam compiled and place the executable in the right spot
# after we have discovered problems with the upstarte service we are not using the upstart service. instead we are going ahead with sensing service only

# we first compile and build the adc module
gcc -c ./adc/adc.c -I./adc -o ./bin/adc.o
gcc -c ./ldr/ldr.c -I./ldr -o./bin/ldr.o
gcc -c ./mq135/mq135.c -I./mq135 -o./bin/mq135.o
gcc -c ./lm35/lm35.c -I./lm35 -o./bin/lm35.o
gcc -c ./alerts/alerts.c -I./alerts -o./bin/alerts.o
gcc -c ./display/display.c -I./display -o./bin/display.o
# this then compresses into a single library that can be archived
ar crv ./bin/libco2.a ./bin/adc.o ./bin/ldr.o ./bin/mq135.o ./bin/lm35.o ./bin/alerts.o ./bin/display.o
ranlib ./bin/libco2.a
#this is where we make that into a running program
gcc ./device.c -o./bin/device -lm -lwiringPi -lwiringPiDev -L ./bin -lco2
gcc ./upstart/upstart.c -o ./bin/upstart -lwiringPi
sudo cp ./upstart/co2sense.service /etc/systemd/system/co2sense.service
sudo systemctl disable co2sense.service
sudo systemctl enable co2sense.service
sudo systemctl daemon-reload
sudo systemctl start co2sense.service
