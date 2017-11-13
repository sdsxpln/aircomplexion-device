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
float ppm_co(int* ok ,int channel){
  *ok =0;
  float voltage= ads115_read_channel(ADS_SLAVE_ADDR, channel, GAIN_ONE, DR_128, ok);
  /*from the extracted voltage we need to then calculate the resistance at the sensor knowing the load resistance
  We are aware that the load and sensor resistance are in series and hence using the Kirchoffs voltage law.
  The 2 resistors form a voltage divider circuit*/
  float vcc  = 5.00;
  float sensorKohm = ((vcc- voltage)*LOAD_RESIS_KOHM)/voltage;
  /*from the characteristic graph that we have in the datasheet we can know that the relation in focus is a logarithmic equation
  log10(Rs/Ro) = mlog10(ppm)+c
  m : slope of the curve
  c : is the constant
  m  = log
  here are the observed points on the graph
  (x1, y1) = (6000, 0.1) - 6000 is a value arrived from visual approximation .. needs to be verified
  (x2, y2) = (100,1)
  this can give us the slope that is requried in the above equation
  m = (log y2-log y1)/(log10 x2- log10 x1) = log10(y2/y1)/log10(x2x/x1) = -0.562381855
  once we have the slope of the equation
  log10(y)= mlog10(x)+c; log10(0.1) = -0.562381855*log10(6000) +c ; c =1.124763709
  >> please note that at all points we have used the log to the base 10
   */
  float ppm  =  powf(10.00, ((log10(sensorKohm/SENSOR_KOHM_CLEAN)-CHAR_CURVE_CONST)/(SLOPE_CHAR_CURVE)));
  printf("%.2f\t\t%.2f\t\t%.3f\n",voltage, sensorKohm, ppm);
  if (*ok ==0){
    return ppm;
  }
  else{return 0.0;}
}
void mq7_shutdown(int gpio){
  heater_off(gpio);
}
float calibrate(int* ok, float ppmco){
  return 0.0;
}
