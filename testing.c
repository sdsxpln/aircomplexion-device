#include<stdio.h>
#include <unistd.h>
#include "./mq7/mq7.h"
void test_mq7_read (){
  int gpio =13 ; //this is the pin at which voltage control is attached
  heater_full_power(gpio);
  sleep(60);
  heater_power(0.27, gpio);
  sleep(90);
  heater_full_power(gpio);
  int ok =0;
  int channel =3;
  float co =ppm_co(&ok ,channel); //this is where you get the reading from sensor
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
