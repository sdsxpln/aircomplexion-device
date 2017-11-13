#ifndef MQ7_H
#define MQ7_H
#define PWM_RANGE 128
#define PWM_DIVISOR 15
#define ADS_SLAVE_ADDR 0x48
#define LOAD_RESIS_KOHM 1 //this is the load resistance across which we do voltage mesurement
#define SENSOR_KOHM_CLEAN 0.55 //this is Ro - the sensor resistance is clean air at 20 deg, 33RH
#define CHAR_CURVE_CONST 1.124763709
#define SLOPE_CHAR_CURVE -0.562381855
/*this gets in the entire 5V power to the sensor
gpio    : the gpio numbered pin that the power supply is attached to*/
void heater_full_power(int gpio);
/*This would adjust the heater power by changing the voltage that is supplied to heater. It is done using the hardware PWM.
We are expeting the heatr circuit to be connected to the one of the pins capable of hardware pwm
powerpercent      :this is the percent of power that is used on the heater, so 1.4V == 0.27% of 5V
gpio        : this is the gpio to which the power supply is connecte to*/
void heater_power(float powerpercent , int gpio);
/*this shuts off the heater entirely
gpio    :this is the gpio pin that is attached to the supply for pwm */
void heater_off(int gpio);
float ppm_co(int* ok ,int channel);
void mq7_shutdown(int gpio);
/*this enables us to get the Rs/Ro ratio , or in effect the resistance of the sensor when in clean air 20 deg 33%RH*/
float calibrate(int* ok, float ppmco);
#endif
