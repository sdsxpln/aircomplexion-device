#include<stdio.h>
#include <unistd.h>
#include "./mq7/mq7.h"
void test_mq7_read (){
  int gpio =13 ; //this is the pin at which voltage control is attached
  heater_full_power(gpio);
  sleep(60);
  heater_power(0.28, gpio); // we know from rpi pin voltage it is 5.11 V so 28% makes 1.43V
  sleep(90);
  heater_full_power(gpio);
  int ok =0;
  int channel =3;
  mq7result result =ppm_co(&ok ,channel); //this is where you get the reading from sensor
  printf("%.2f\t\t%.2f\t\t%.2f\n", result.volts, result.sensor_kohms, result.co_ppm);
  heater_off(gpio);
}
int main(int argc, char const *argv[]) {
  // testing the function for atleast 20 times - noting that each of the cycle is 90+60 secs minimum
  size_t i=0;
  for (i = 0; i < 20; i++) {
    test_mq7_read();
    sleep(3);
  }
  return 0;
}
