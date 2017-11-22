/*
date          : 20-NOV-2017
author        : kneerunjun@gmail.com
purpose       : this is to write the sensing code all over again , since we are  making the sensors module  separate
compilation   : gcc ./device.c -o ./bin/device -lwiringPi -lwiringPiDev -lm -L./adc -ladc
run           : ./bin/sensing.c
*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>    // exit, delay
#include <wiringPi.h>
#include<lcd.h>
#include <string.h>
#include<signal.h>
#include <pthread.h>
#include "./ldr/ldr.h"
#include "./display/display.h"
#include "./lm35/lm35.h"
#include "./alerts/alerts.h"
#include "./mq135/mq135.h"
#include "./adc/adc.h"
// #include "./mq7/mq7.h"

#define DARK_VOLTS 0.806
#define BRIGHT_VOLTS 2.013
#define SECSTOMICROSECS 1000000
#define LDR_CHANNEL 3
#define LM35_CHANNEL 2
#define MQ135_CHANNEL 1
#define RED_GPIO 16
#define BLUE_GPIO 20
#define BUZZ_GPIO 21

void* ldr_loop(void* argc); //light sensing loop
void* display_loop(void* argc); //light sensing loop
void* temp_loop(void* argc); //light sensing loop
void* alert_loop(void* argc); //light sensing loop
void* co2_loop(void* argc);
typedef struct {
  float co2_ppm;
  float temp_celcius;
  float light_cent;
  float co_ppm;
  char err[128];
}ambience; //this structure represents the conditions collectively that are thread safe
ambience ambientnow = {.co2_ppm=0, .temp_celcius=0, .light_cent=0, .co_ppm=0}; //presetting the values in the structure
pthread_t tids[4];
pthread_mutex_t lock;
// More often than not , yo find the need to check the readings from the ADS - this can give you the readings correctly
int main(int argc, char const *argv[]) {
  float readings[4];
  while (1) {
    if (ads115_read_all_channels(0x48,readings)!=0) {
      perror("This was not well read by ads channel");
    }
    printf("%.4f\t\t%.4f\t\t%.4f\t\t%.4f\n", readings[0],readings[1],readings[2], readings[3]);
    // float channel;
    // if (ads115_read_channel(0x48,2,GAIN_TWO,DR_128, &channel)!=0) {
    //   perror("Error reading a single channel");
    // }
    // printf("Channel 2: %.3f\n", channel);
    // lm35Result result;
    // if(airtemp_now(LM35_CHANNEL,CELCIUS, &result)!=0){
    //   perror("Error reading the temperature..");
    // }
    // printf("Temp: %.3f\n", result.temp);
    sleep(2);
  }
  return 0;
}
int main_arc(int argc, char const *argv[]) {
  int ok =0;
  wiringPiSetupGpio();
  // instantiating the lock here
  if (pthread_mutex_init(&lock, NULL)!=0) {
    printf("Error starting a new thread, exiting application\n");
    return 1;
  }
  if (pthread_create(&tids[0], NULL,&ldr_loop, NULL)!=0) {
    printf("Failed to start activity thread\n");
    return 1;
  }
  if (pthread_create(&tids[1], NULL,&display_loop, NULL)!=0) {
    printf("Failed to start activity thread\n");
    return 1;
  }
  if (pthread_create(&tids[2], NULL,&temp_loop, NULL)!=0) {
    printf("Failed to start activity thread\n");
    return 1;
  }
  if (pthread_create(&tids[3], NULL,&co2_loop, NULL)!=0) {
    printf("Failed to start activity thread\n");
    return 1;
  }
  size_t i;
  int array_sz = sizeof(tids)/sizeof(pthread_t);
  for (i = 0; i < array_sz; i++) {
    pthread_join(tids[i], NULL);
  }
  return 0;
  // flushing the lock here
  pthread_mutex_destroy(&lock);
  return 0;
}
void* co2_loop(void* argc){
  printf("We are now starting the co2 loop\n");
  while (1) {
    usleep(4*SECSTOMICROSECS);
  }
}
void* alert_loop(void* argc){
  setup_alert(BLUE_GPIO,RED_GPIO,BUZZ_GPIO);
  int ok =0;
  while (1) {
    alert(&ok,0,0);
    usleep(4*SECSTOMICROSECS);
    alert(&ok,1,0);
    usleep(4*SECSTOMICROSECS);
    alert(&ok,2,0);
    usleep(4*SECSTOMICROSECS);
  }
  clear_all_alerts();
}
void* temp_loop(void* argc){
  lm35Result result;
  while (1) {
    if(airtemp_now(LM35_CHANNEL,CELCIUS, &result)!=0){
      perror("Error reading the temperature..");
    }
    pthread_mutex_lock(&lock);
    ambientnow.temp_celcius = result.temp;
    pthread_mutex_unlock(&lock);
    usleep(4*SECSTOMICROSECS);
  }
}
void* display_loop(void* argc){
  char message[80];
  setup_lcd_4bitmode(2, 16, 17,27,18,23,24,25);
  while (1) {
    pthread_mutex_lock(&lock);
    sprintf(message, "L:%.2f T:%.2f", ambientnow.light_cent, ambientnow.temp_celcius);
    pthread_mutex_unlock(&lock);
    lcd_clear();
    lcd_message(message);
    usleep(4*SECSTOMICROSECS); //refresh the display every 1 second
  }
  lcd_clear();
}
void* ldr_loop(void* argc){
  ldrResult result;
  while (1) {
    if (light_percent(LDR_CHANNEL, BRIGHT_VOLTS ,DARK_VOLTS,&result)!=0){
      perror("ldr loop: error reading the light conditions");
    }
    // printf("%.3f\n",result.volts);
    pthread_mutex_lock(&lock);
    ambientnow.light_cent = result.light;
    pthread_mutex_unlock(&lock);
    usleep(4*SECSTOMICROSECS);
  }
}
