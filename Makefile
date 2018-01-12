# -*- Make -*-

all: aircomplexion
testing:
	gcc -c -g -Wall -DJRNL=1 journal.c -I ./include -o ./lib/journal.o
	gcc -g testing.c -o ./bin/testing -I ./include ./lib/journal.o

aircomplexion: sensors
	ar crv ./lib/libaircomplexion.a ./lib/*
	ranlib ./lib/libaircomplexion.a
	gcc sensing.c -Wall -DTEST -I ./include/ -o ./bin/sensing \
	-lm -lwiringPi -lwiringPiDev -lpthread -lcurl -L ./lib -laircomplexion
	gcc upstart.c -I ./include -o ./bin/upstart -lwiringPi -lwiringPiDev
	# /usr/local/bin/EensyMachines/aircomplexion/upstart
sensors:	core
	gcc -c ldr.c -I ./include -o ./lib/ldr.o
	gcc -c lm35.c -I ./include -o ./lib/lm35.o
	gcc -c mq135.c -I ./include -o ./lib/mq135.o
	gcc -c mq7.c -I ./include -o ./lib/mq7.o
core:
	gcc -c display.c -I ./include -o ./lib/display.o
	gcc -c alerts.c -I ./include -o ./lib/alerts.o
	gcc -c adc.c -I ./include -o ./lib/adc.o
	gcc -c license.c -I ./include -o ./lib/license.o
	gcc -c uplink.c -I ./include -o ./lib/uplink.o
	gcc -c -Wall -DJRNL=1 journal.c -I ./include -o ./lib/journal.o
