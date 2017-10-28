#ifndef LM35_H
#define LM35_H
typedef enum {CELCIUS=-0, FARENHEIT=1} TEMPUNITS; //this gives the units in which the temp has be to be displayed
float airtemp_now(int*ok,int adschn, TEMPUNITS unit);
#endif
