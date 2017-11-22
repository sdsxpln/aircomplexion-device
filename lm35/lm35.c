
#include"lm35.h"
#include"../adc/adc.h"
#include<stdio.h>
int airtemp_now(int adschn, TEMPUNITS unit, lm35Result* result){
  float volts;
  if (ads115_read_channel(0x48,2,GAIN_TWO,DR_128, &volts)!=0) {
    perror("Error reading a single channel");
  }
  // printf("From inside lm35 : %.3f\n", volts);
  result ->volts = volts;
  result ->temp  = volts*100;
  return 0;
}
