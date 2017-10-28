#ifndef MQ135_H
#define MQ135_H
// refer this for slope and intercept calculations http://www.instructables.com/id/Arduino-Air-Monitor-Shield-Live-in-a-Safe-Environm/
#define CO2_Y_INTERCEPT 0.7597917824
#define CO2_SLOPE -0.370955166
//this is the load resistance on the sensor, you would perhaps have to measure on the device
#define LOAD_RESISTANCE_KOHMS 2.5
#define CO2_PPM_NOW 402.73 // refer https://www.co2.earth/
#define VDD 5.00 //this is assumed that we are connecting the sensor to this much of volts
float ppm_co2(int* ok, int adschn, int fcalibrate);
#endif
