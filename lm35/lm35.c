
#include"lm35.h"
#include"../adc/adc.h"
#include<stdio.h>
float airtemp_now(int*ok,int adschn, TEMPUNITS unit){
  float volts =ads115_read_channel(0x48,adschn, GAIN_EIGHT, DR_128,ok);
  // We have observed that the voltage reading from the ads channel itself
  if (*ok==0 && volts!=0) {
    if (unit == CELCIUS) {
      *ok =0;
      return volts *100;
    }
    else{*ok=0; return (volts*100)*(9/5) +32;}
  }
  else{
    *ok=-1;
    perror("lm35.c:Failed to get reading from the ADS channel!");
    return 0;
  }
}
