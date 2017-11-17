/*
date          : 25-AUG-2017
author        : kneerunjun@gmail.com
purpose       : to try measuring Analog inputs on Rpi using an ADS1115 over I2C and then connect a MQ135 Co2 measurement unit
compilation   : gcc ./device.c -o ./bin/device -lwiringPi -lwiringPiDev -lm -L./adc -ladc
run           : ./bin/i2ctest
*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>    // exit, delay
#include <wiringPi.h>
#include<lcd.h>
#include <string.h>
#include<signal.h>
#include "./mq135/mq135.h"
#include "./ldr/ldr.h"
#include "./lm35/lm35.h"
#include "./alerts/alerts.h"
#include "./display/display.h"
#include <pthread.h>

#define RS 9
#define E 11
#define D0 14
#define D1 15
#define D2 18
#define D3 23
//trigger this for the visual and sonic indication
#define RED_GPIO 17
#define BLUE_GPIO 27
#define BUZZ_GPIO 22
#define BTN_SHUTDOWN 21
#define BTN_RESET 20
// refer to https://www.co2.earth/ for knowing the current concentration of C02
/* http://www.instructables.com/id/Arduino-Air-Monitor-Shield-Live-in-a-Safe-Environm
this actually can be seen from the characteristics of this sensor , for which you would have to rfer to datasheet
but then the above site makes it much more easier and gives values directly you can use
equation    : log(Rs/Ro) = m*log(ppm)+c
for knowing the various levels of Co2 https://www.kane.co.uk/knowledge-centre/what-are-safe-levels-of-co-and-co2-in-rooms
*/
#define LOOP_MAX 3600
#define LOOP_SLEEP_SECS 5
// these extremeties need to adjusted / calibrated.
// this needs to be tested on a restart.
#define DARK_VOLTS 1.6851
#define BRIGHT_VOLTS 2.2831
#define SECSTOMICROSECS 1000000
pthread_mutex_t lock ; //lock for the mutex.
typedef struct {
  float co2_ppm;
  float temp_celcius;
  float light_cent;
  char err[128];
}ambience; //this structure represents the conditions collectively that are thread safe
ambience ambientnow = {.co2_ppm=0, .temp_celcius=0, .light_cent=0, .err=""}; //presetting the values in the structure
pthread_t tids[10];
void on_interrupt(int signal){
  digitalWrite(BLUE_GPIO, LOW);
  digitalWrite(RED_GPIO, LOW);
  digitalWrite(BUZZ_GPIO, LOW);
  lcd_clear();
  exit(0);
}
/*This runs the sensing loop and deposits the value of the co2 in the structure above*/
void* senseloop_co2(void* arg){
  int ok =0;
  setup_alert(BLUE_GPIO, RED_GPIO, BUZZ_GPIO);
  while (1) {
    pthread_mutex_lock(&lock);
    ambientnow.co2_ppm = ppm_co2(&ok, 0, 0);//measurement from the sensor
    if(ok!=0){perror("device.c: failed to get the co2 footprint");}
    if (ambientnow.co2_ppm <=700.00) {alert(&ok, 0, 0);}
    else if(ambientnow.co2_ppm>700.00 && ambientnow.co2_ppm<=1500.00){alert(&ok, 1, 0);}
    else if(ambientnow.co2_ppm>1500.00){alert(&ok, 2, 0);}
    pthread_mutex_unlock(&lock);
    //sleep(secs) does not work since this is a thread not a process.
    usleep(2*SECSTOMICROSECS);
  }
}
/*this just runs a loop in that keeps picking up values from structure to update the display lcd*/
void* display_loop(void* arg){
  setup_lcd_4bitmode(2,16,RS,E,D0,D1,D2,D3);
  lcd_message("Sensing..");
  while(1) {
    pthread_mutex_lock(&lock);
    display_readings(ambientnow.temp_celcius, ambientnow.light_cent*100, ambientnow.co2_ppm);
    pthread_mutex_unlock(&lock); //done displaying release lock on structure
    usleep(2*SECSTOMICROSECS); //we try to refresh the display every 2 seconds
  }
}
void* senseloop_temp(void* arg){
  int ok =0;
  while (1) {
    pthread_mutex_lock(&lock);
    ambientnow.temp_celcius = airtemp_now(&ok,1, CELCIUS);
    // for the bug metioned - this temp reading is still not as expeted.
    if(ok!=0){perror("We have a problem reading the temperature channel on the ADS");}
    pthread_mutex_unlock(&lock);
    usleep(5*SECSTOMICROSECS);// we intend to measure the temp every 5 seconds
  }
}
void* senseloop_light(void* arg){
  int ok =0;
  while (1) {
    pthread_mutex_lock(&lock);
    ambientnow.light_cent = light_percent(&ok, 2, BRIGHT_VOLTS, DARK_VOLTS);
    if(ok!=0){perror("We have a problem reading the light intensity");}
    pthread_mutex_unlock(&lock);
    usleep(3*SECSTOMICROSECS);// we intend to measure the light every 3 seconds
  }
}
int main(int argc, char const *argv[]) {
  int ok =0;
  // register a signal
  // here we are testing only for the SIGINT
  signal(SIGINT, &on_interrupt);
  signal(SIGTERM, &on_interrupt);
  // this is being setup from the upstart service , re setting here woudl lead to some problem
  wiringPiSetupGpio();
  // instantiating the lock here
  if (pthread_mutex_init(&lock, NULL)!=0) {
    printf("Error starting a new thread, exiting application\n");
    return 1;
  }
  // spins out threads for each of the sensing activities and display as a separate one
  if (pthread_create(&tids[0], NULL,&display_loop, NULL)!=0) {
    printf("Failed to start activity thread\n");
    return 1;
  }
  if (pthread_create(&tids[1], NULL,&senseloop_co2, NULL)!=0) {
    printf("Failed to start activity thread\n");
    return 1;
  }
  if (pthread_create(&tids[2], NULL,&senseloop_temp, NULL)!=0) {
    printf("Failed to start activity thread\n");
    return 1;
  }
  if (pthread_create(&tids[3], NULL,&senseloop_light, NULL)!=0) {
    printf("Failed to start activity thread\n");
    return 1;
  }
  // joining all the threaded activities
  size_t i;
  for (i = 0; i < 4; i++) {
    pthread_join(tids[i], NULL);
  }
  // flushing the lock here
  pthread_mutex_destroy(&lock);
  // prep_hw_shutdown();
  return 0;
}
