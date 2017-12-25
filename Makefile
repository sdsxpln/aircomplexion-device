# -*- Make -*-

all:	upstart_as_service	upstart	sensing	sensing_ranlib	sensing_ar	mq7_o	mq135_o	alerts_o	lm35_o	display_o	ldr_o	adc_o uplink_o
test: testing

upstart_as_service:	upstart
	sudo cp ./upstart/co2sense.service /etc/systemd/system/co2sense.service
	sudo systemctl disable co2sense.service
	sudo systemctl enable co2sense.service
	sudo systemctl daemon-reload
	sudo systemctl stop co2sense.service
	sudo systemctl start co2sense.service

upstart: sensing
	gcc ./upstart/upstart.c -o./bin/upstart -lwiringPi

sensing: sensing_ranlib
	gcc ./sensing.c -o./bin/sensing -lm -lwiringPi -lwiringPiDev -lpthread -lcurl -L./bin -lsensing

sensing_ranlib: sensing_ar
	ranlib ./bin/libsensing.a

sensing_ar:	mq7_o	mq135_o	alerts_o	lm35_o	display_o	ldr_o	adc_o uplink_o
	ar crv ./bin/libsensing.a ./bin/adc.o ./bin/ldr.o ./bin/display.o ./bin/lm35.o ./bin/alerts.o ./bin/mq135.o ./bin/mq7.o ./bin/uplink.o

testing: uplink_o
	gcc -g ./testing.c -o./bin/testing ./bin/uplink.o -L./bin -lcurl
	gdb ./bin/testing

mq7_o:	adc_o
	gcc -c ./mq7/mq7.c -I./mq7 -o./bin/mq7.o

mq135_o:	adc_o
	gcc -c ./mq135/mq135.c -I./mq135 -o./bin/mq135.o

alerts_o:
	gcc -c ./alerts/alerts.c -I./alerts -o./bin/alerts.o

lm35_o:	adc_o
	gcc -c ./lm35/lm35.c -I./lm35 -o./bin/lm35.o

display_o:	adc_o
	gcc -c ./display/display.c -I./display -o./bin/display.o

ldr_o:	adc_o
	gcc -c ./ldr/ldr.c -I./ldr -o./bin/ldr.o

adc_o:
	gcc -c ./adc/adc.c -I./adc -o ./bin/adc.o

uplink_o:
	gcc -g -c ./uplink/uplink.c -I./uplink -o ./bin/uplink.o
