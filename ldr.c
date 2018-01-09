#include "ldr.h"

/*this function returns the temperature of the room
@ok         :status of the function
@adschn     :this is the channel to which the LDR is attached
@voltbright :voltage when it is the brightest
@voltdark   :voltage when it is the darkest*/
int light_percent(int adschn, float voltbright, float voltdark, ldrResult* result){
  result->volts=-1;
  result->light=-1;
  result->max_min[0]=voltdark;
  result->max_min[1]=voltbright;
  if (voltdark >=voltbright) {
    perror("ldr.c: the extremeties of the light voltages are invalid.");
    result->ok =-1;
    return -1;
  }
  int ok=0;
  float volts;
  if (ads115_read_channel(0x48, adschn, GAIN_ONE, DR_128,&(result->volts))!=0){
    perror("LDR : Failed to get voltage reading");
    return -1 ;
  };
  float normVolts; //this is the normalized volts between the maxima and minima
  if (result->volts >0) {
    if (result->volts > voltbright) {normVolts = voltbright;}
    if (result->volts < voltdark){normVolts=voltdark;}
    else{normVolts = result->volts;}
    result->light= ((float)(normVolts-voltdark)/(voltbright-voltdark));
    result->ok=0;
    return 0;
  }
}
