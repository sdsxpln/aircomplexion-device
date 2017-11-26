#ifndef MQ7_H
#define MQ7_H
#define PWM_RANGE 128
#define PWM_DIVISOR 15
#define ADS_SLAVE_ADDR 0x48
//this is the load resistance across which we do voltage mesurement
#define LOAD_RESIS_KOHM 1
/*We currently dont have any basis to experimentally prove that the resistance of the sensor in clean air would be this.
From various internet sources we have observed that in a room (as mine right now) we get almost about 0.9 ppm of C0.
This also is in agreement with citations saying that normal healthy rooms have between 0.5 to 5 ppm of CO */
#define SENSOR_KOHM_CLEAN 0.119 //0.805//0.2633 //this is Ro - the sensor resistance is clean air at 20 deg, 33RH
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
#define CHAR_CURVE_CONST 1.124763709
#define SLOPE_CHAR_CURVE -0.562381855
typedef struct {
  float volts;
  float sensor_kohms;
  float co_ppm;
}mq7result;
/*this gets in the entire 5V power to the sensor
gpio    : the gpio numbered pin that the power supply is attached to*/
void heater_full_power(int gpio, int npn_invert);
/*This would adjust the heater power by changing the voltage that is supplied to heater. It is done using the hardware PWM.
We are expeting the heatr circuit to be connected to the one of the pins capable of hardware pwm
powerpercent      :this is the percent of power that is used on the heater, so 1.4V == 0.27% of 5V
gpio        : this is the gpio to which the power supply is connecte to*/
void heater_power(float powerpercent,int gpio,int npn_invert);
/*this shuts off the heater entirely
gpio    :this is the gpio pin that is attached to the supply for pwm */
void heater_off(int gpio, int npn_invert);
int ppm_co(int channel, mq7result* result);
void mq7_shutdown(int gpio,int npn_invert);
/*this enables us to get the Rs/Ro ratio , or in effect the resistance of the sensor when in clean air 20 deg 33%RH*/
int calibrate(int adschn, float ppmco, float* ro);
#endif
