#include <stdio.h>
#include <unistd.h>
#include "mq7.h"
#include "../alerts/alerts.h"
#define RED_GPIO 16
#define BLUE_GPIO 20
#define BUZZ_GPIO 21
int main(int argc, char const *argv[]) {
  float ro;
  int ok =0;
  float ppm_co_known = 0.5;
  setup_alert(BLUE_GPIO,RED_GPIO,BUZZ_GPIO);
  while (1) {
    alert(&ok, 0,1);
    printf("Heater in full power mode..\n");
    heater_full_power(12,1);
    sleep(60);
    printf("Heater now in partial power\n");
    alert(&ok, 1,1);
    heater_power(0.10 , 12, 1);
    sleep(90);
    alert(&ok, 2,1);
    printf("Ready for calibration measurements..\n");
    if (calibrate(0,ppm_co_known,&ro)!=0) {
      perror("failed to run calibration");
    }
    printf("Ro : %.3f\n", ro);
  }
  return 0;
}
