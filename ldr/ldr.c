#include "../ldr/ldr.h"
#include "../adc/adc.h"

/*this function returns the temperature of the room
@ok         :status of the function
@adschn     :this is the channel to which the LDR is attached
@voltbright :voltage when it is the brightest
@voltdark   :voltage when it is the darkest*/
float light_percent(int* ok, int adschn, float voltbright, float voltdark){
  *ok = 0; //to start with everything is ok
  if (voltdark >=voltbright) {
    // this is the case that is not acceptable
    perror("ldr.c: the extremeties of the light voltages are invalid.");
    *ok=-1; return 0;
  }
  float volts = ads115_read_channel(0x48, adschn, GAIN_ONE, DR_128,ok);
  if (*ok==0 && volts >0) {
    if (volts > voltbright) {volts = voltbright;}
    if (volts < voltdark){volts=voltdark;}
    float lightCent= ((float)(volts- voltdark)/(voltbright-voltdark));
    *ok=0;
    return lightCent;
  }
  else{perror("ldr.c: Error reading the voltage from the ADC chip");*ok=-1; return 0;}
}
