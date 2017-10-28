#include<stdio.h>
#include <math.h>
#include "../adc/adc.h" //this is for accessing the analog readings from the ADS1115
#include"../mq135/mq135.h"
/*this is the function capable of returning the co2 reading in parts per million
@ok         :denotes the status of the calcualtions and overall function status -check to for < 0 to know the presence of exceptions
@adschannel :the channel where the mq135 is actually attached.
@fcalibrate :this would enforce priming the device all over again, Ro value would be recalibrated
*/
float ppm_co2(int* ok, int adschn, int fcalibrate){
  static float Ro =-1.00; //this is the indicator if negative then the device has not been calibrated
  /*ratioRsRo     : ratio of sensor resistance in actual v/s ratio of sensor resistance in free air
  Vrl             : Voltage across the load resistance
  Rs              : Sensor resistance*/
  float ratioRsRo=1, Vrl=0.00, Rs =0.00;
  if (fcalibrate==1) {
    Ro =-1;
  }
  if (Ro<=0) {
    // from the current Co2 in the atmosphere we are getting the Rs/Ro ratio of resistance
    ratioRsRo=pow(10,((CO2_SLOPE*(log10(CO2_PPM_NOW))+CO2_Y_INTERCEPT)));
    /*Voltage across the load resistance can be measured using the ADS - is the voltage output from the sensor
    observe the gain we are setting to : +-2.048 */
    Vrl=ads115_read_channel(0x48,adschn, GAIN_TWO, DR_128,ok);
    if(*ok!=0 || Vrl ==0){perror("mq135.c:failed priming of the sensor"); return 0;}
    /*please read carefully this is where I dont remember how the calculations are made
    Rt = Rs + Rl .. just the total resistance
    Rs = Rt-Rl
    Vrl  = V*(Rl/Rt) .. this from ohms law
    Rt = (V*Rl)/Vrl
    Rs = ((V*Rl)/Vrl)-Rl ... and that gives you the the sensor resistance
    Also please note we are assuming all the resistance in KOHMS since there is a ratio that we are dealing here it does not cause any problem
    */
    Rs=(VDD * LOAD_RESISTANCE_KOHMS/Vrl)- LOAD_RESISTANCE_KOHMS; //sensor resistance
    Ro=Rs/ratioRsRo;//resistance of sensor at clean air conditions
  }
  // from here on we have the regular ppm calculations
  Vrl=ads115_read_channel(0x48,adschn, GAIN_TWO, DR_128,ok); //measuring voltage ouput
  if(*ok!=0 || Vrl ==0){perror("mq135.c: failed to read output of sensor"); return 0;}
  Rs=(VDD * LOAD_RESISTANCE_KOHMS/Vrl)- LOAD_RESISTANCE_KOHMS;
  if (Ro >0) {
    float ppm = pow(10,((log10(Rs/Ro)-CO2_Y_INTERCEPT)/CO2_SLOPE));
    *ok=0;
    return ppm; // final value of the co2 content in part per million
  }
  else{perror("mq135.c :The sensor has not been primed"); *ok=-1; return 0;}
}
