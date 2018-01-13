#include "mq7.h"

void heater_full_power(int gpio,int npn_invert){
  // wiringPiSetupGpio();
  pinMode(gpio, OUTPUT);
  // npn transistor may act like an inverotr hence the provision
  // we may have a setup where the npn may be parallel to the sensor load , in which case turning the heater on is actually lowering the gpio
  if (npn_invert==0) {digitalWrite(gpio,HIGH);}
  else{digitalWrite(gpio,LOW);}
  // printf("Heater in full power mode..\n");
}
void heater_off(int gpio, int npn_invert){
  // wiringPiSetupGpio();
  pinMode(gpio, OUTPUT);
  if(npn_invert==0){digitalWrite(gpio,LOW);}
  else{digitalWrite(gpio,HIGH);}
  // printf("Heater is now turned off\n");
}
void heater_power(float powerpercent , int gpio, int npn_invert){
  // wiringPiSetupGpio();
  pinMode(gpio, PWM_OUTPUT);
  pwmSetMode(PWM_MODE_MS);
  pwmSetRange(PWM_RANGE);
  pwmSetClock(PWM_DIVISOR);
  // printf("Hardware setup for pwm done\n");
  if(npn_invert==0){pwmWrite(gpio,powerpercent*PWM_RANGE);}
  else{pwmWrite(gpio,(1-powerpercent)*PWM_RANGE);}
  // printf("Issued pwm signal\n");
  // printf("Heater now in partial power %.3f\n", powerpercent);
}
/*
This gets you the calcualted ppm content for co
channel   : ADC channel you want to read from
result    : structure to which all the result values would be updated.
*/
int ppm_co(int channel, mq7result* result){
  if (ads115_read_channel(ADS_SLAVE_ADDR, channel, GAIN_TWO, DR_128, &(result->volts))!=0) {
    // error getting voltage from channel
    perror("mq7/ppm_co:Failed to get voltage reading from ADC");
    result->volts = -1;
    result ->sensor_kohms = -1;
    result->co_ppm = -1;
    return -1;
  }
  /*from the extracted voltage we need to then calculate the resistance at the sensor knowing the load resistance
  We are aware that the load and sensor resistance are in series and hence using the Kirchoffs voltage law.
  The 2 resistors form a voltage divider circuit*/
  float vcc  = 5.00;
  result->sensor_kohms = ((vcc- result->volts)*LOAD_RESIS_KOHM)/result->volts;
  result->co_ppm =  powf(10.00, ((log10(result->sensor_kohms/SENSOR_KOHM_CLEAN)-CHAR_CURVE_CONST)/(SLOPE_CHAR_CURVE)));
  return 0;
}
void mq7_shutdown(int gpio, int npn_invert){
  heater_off(gpio,npn_invert);
}
/*this method is used to recalibrate the sensor hardware
One would have to run the sensor in an environment with known CO content.
This environment with the known CO content then can help us back calc the Ro when we get the sensor resistance from thevoltage OUTPUT
ppmco       : this is co content in the known environment
Ro          : this is the output . sensor resistance in 20 deg c and 33% RH
adschn      : channel at which the ads should be read
here we are assuming the other graph params are ok - they are observed on the datasheet and obviously any error in reading them would be detrimental to the output here.
 */
int calibrate(int adschn, float ppmco, float* ro){
  float log_rs_ro = (SLOPE_CHAR_CURVE*log10(ppmco))+CHAR_CURVE_CONST;
  float rs_ro = powf(10.00, log_rs_ro);
  float vrl;
  float vcc  = 5.00;
  // now getting the reading from teh ads channel so that we have the sensor resistance
  if (ads115_read_channel(ADS_SLAVE_ADDR, adschn, GAIN_TWO, DR_128, &vrl)!=0) {
    perror("mq7/calibrate:failed to read the ads channel for the voltage");
    return -1; //unable to read the ads channel would mean we would have to abandon the calibration
  }
  printf("Volatge recorded was %.3f\n",vrl );
  float rs = ((vcc- vrl)*LOAD_RESIS_KOHM)/vrl;
  if (rs_ro<=0) {
    perror("mq7/calibrate: Invalid value for sensor resistance ratio, check the environement for excessive co content");
    return -1;
  }
  *ro  =rs/rs_ro;
  // result ->sensor_kohms = ro;
  // result ->volts = vrl;
  // result->co_ppm =ppmco; // this is calibration
  return 0;
}
