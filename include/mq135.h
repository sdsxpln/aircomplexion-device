#include<stdio.h>
#include <math.h>
#include "adc.h" //this is for accessing the analog readings from the ADS1115
#ifndef MQ135_H
#define MQ135_H
// refer this for slope and intercept calculations http://www.instructables.com/id/Arduino-Air-Monitor-Shield-Live-in-a-Safe-Environm/
#define CO2_Y_INTERCEPT 0.7597917824
#define CO2_SLOPE -0.370955166
//this is the load resistance on the sensor, you would perhaps have to measure on the device
#define LOAD_RESISTANCE_KOHMS 2.5
#define CO2_PPM_NOW 406.75 // refer https://www.co2.earth/
#define VOLT_PPM_NOW 0.9035 // << this is the observed voltage when considering ppm now
#define VDD 5.00 //this is assumed that we are connecting the sensor to this much of volts
// This structure signifies the result from each of reading.
// use this to send out results
typedef struct {
  float sensorKohms;
  float ppmCo2;
  float volts;
}mq135Result;
int ppm_co2(int adschn, int fcalibrate, mq135Result* result);

#endif
