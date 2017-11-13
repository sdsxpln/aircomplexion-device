#include<stdio.h>
#include "mq7.h"
#include<wiringPi.h>
#include "../adc/adc.h"
#include <math.h>
void heater_full_power(int gpio){
  wiringPiSetupGpio();
  pinMode(gpio, OUTPUT);
  digitalWrite(gpio,HIGH);
  printf("Heater in full power mode..\n");
}
void heater_off(int gpio){
  wiringPiSetupGpio();
  pinMode(gpio, OUTPUT);
  digitalWrite(gpio,LOW);
  printf("Heater is now turned off\n");
}
void heater_power(float powerpercent , int gpio){
  wiringPiSetupGpio();
  pinMode(gpio, PWM_OUTPUT);
  pwmSetMode(PWM_MODE_MS);
  pwmSetRange(PWM_RANGE);
  pwmSetClock(PWM_DIVISOR);
  pwmWrite(gpio,powerpercent*PWM_RANGE);
  printf("Heater now in partial power %.3f\n", powerpercent);
}
//We are just leaving this undone for the time being

mq7result ppm_co(int* ok ,int channel){
  *ok =0;
  mq7result result;
  float voltage= ads115_read_channel(ADS_SLAVE_ADDR, channel, GAIN_ONE, DR_128, ok);
  /*from the extracted voltage we need to then calculate the resistance at the sensor knowing the load resistance
  We are aware that the load and sensor resistance are in series and hence using the Kirchoffs voltage law.
  The 2 resistors form a voltage divider circuit*/
  if(*ok ==0){
    float vcc  = 5.00;
    float sensorKohm = ((vcc- voltage)*LOAD_RESIS_KOHM)/voltage;
    float ppm  =  powf(10.00, ((log10(sensorKohm/SENSOR_KOHM_CLEAN)-CHAR_CURVE_CONST)/(SLOPE_CHAR_CURVE)));
    if (*ok ==0){
      // flywheeling the entire result in an object
      result.volts =voltage;
      result.sensor_kohms = sensorKohm;
      result.co_ppm = ppm;
      result.ok  = *ok;
      result.err_msg = "";
    }
  }
  else{
    // there was problem reading the ads channel
    result.volts =-1;
    result.sensor_kohms = -1;
    result.co_ppm = -1;
    result.ok  = *ok;
    result.err_msg = "Problem reading the ads channel";
  }
  return result;
}
void mq7_shutdown(int gpio){
  heater_off(gpio);
}
float calibrate(int* ok, float ppmco){
  return 0.0;
}
