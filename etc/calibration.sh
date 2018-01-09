#!/bin/sh
gcc -c calibration.c -o../bin/calibration.o -I./ -I../alerts/
gcc ../bin/calibration.o -L../bin -lsensing -lwiringPi -lwiringPiDev -lm -o../bin/calibration
sudo ../bin/calibration
