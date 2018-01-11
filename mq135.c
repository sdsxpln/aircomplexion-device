#include"mq135.h"
/*this is the function capable of returning the co2 reading in parts per million
@ok         :denotes the status of the calcualtions and overall function status -check to for < 0 to know the presence of exceptions
@adschannel :the channel where the mq135 is actually attached.
@fcalibrate :this would enforce priming the device all over again, Ro value would be recalibrated
*/
int ppm_co2(int adschn, int fcalibrate, mq135Result* result){
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
    /*please read carefully this is where I dont remember how the calculations are made
    Rt = Rs + Rl .. just the total resistance
    Rs = Rt-Rl
    Vrl  = V*(Rl/Rt) .. this from ohms law, remember this is not the voltage measured !!
    this is the observed voltage when the conditions were of CO2_PPM_NOW, so this then becomes
    VOLT_PPM_NOW, I have made this mistake in the past and has cost me dearly
    Rt = (V*Rl)/Vrl
    Rs = ((V*Rl)/Vrl)-Rl ... and that gives you the the sensor resistance
    Also please note we are assuming all the resistance in KOHMS since there is a ratio that we are dealing here it does not cause any problem
    */
    Rs=(VDD * LOAD_RESISTANCE_KOHMS/VOLT_PPM_NOW)- LOAD_RESISTANCE_KOHMS; //sensor resistance
    Ro=Rs/ratioRsRo;//resistance of sensor at clean air conditions
  }
  // from here on we have the regular ppm calculations
  /*Voltage across the load resistance can be measured using the ADS - is the voltage output from the sensor
  observe the gain we are setting to : +- 4.02 , we have observed that mq135 can move upto this level*/
  if(ads115_read_channel(0x48,adschn, GAIN_ONE, DR_128,&Vrl)!=0){
    perror("mq135: error reading the ads channel");
    return -1;
  }
  if(Vrl ==0){perror("mq135.c: failed to read output of sensor"); return -1;}
  result ->volts  = Vrl;
  Rs=(VDD * LOAD_RESISTANCE_KOHMS/Vrl)- LOAD_RESISTANCE_KOHMS;
  result->sensorKohms =Rs;
  if (Ro >0) {
    float ppm = pow(10,((log10(Rs/Ro)-CO2_Y_INTERCEPT)/CO2_SLOPE));
    result->ppmCo2=ppm;
    return 0; // final value of the co2 content in part per million
  }
  else{perror("mq135.c :The sensor has not been primed"); return -1;}
}
