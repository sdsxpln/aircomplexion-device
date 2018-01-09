#include"adc.h"
#include<stdio.h>
#ifndef LM35_H
#define LM35_H
typedef enum {CELCIUS=0, FARENHEIT=1} TEMPUNITS; //this gives the units in which the temp has be to be displayed
typedef struct {float volts; float temp;}lm35Result;
int airtemp_now(int adschn, TEMPUNITS unit, lm35Result* result);
#endif
