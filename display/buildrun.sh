#!/bin/sh
gcc -c display.c -I. -o./display.o
gcc display.o -o./display -lwiringPiDev -lwiringPi
sudo ./display
