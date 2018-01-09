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
#include "./mq7/mq7.h"
#include "./uplink/uplink.h"

#define DARK_VOLTS 0.7899
#define BRIGHT_VOLTS 2.2559
/*light measuring voltage is observed on 22 NOV 2017 on a bright sunny morning with ample light in the laboratory*/
#define SECSTOMICROSECS 1000000
#define LDR_CHANNEL 3
#define LM35_CHANNEL 2
#define MQ135_CHANNEL 1
#define MQ7_CHANNEL 0
#define RED_GPIO 16
#define BLUE_GPIO 20
#define BUZZ_GPIO 21
#define MQ7_HEATER_GPIO 12
#define LCD_ROWS 2
#define LCD_COLS 16
#define LCD_RS 17
#define LCD_E 27
#define LCD_D0 8
#define LCD_D1 23
#define LCD_D2 24
#define LCD_D3 25
#define Co2_LVL_WARN 700 //ppm level at which warning message should be sounded
#define Co2_LVL_ALARM 1500 //level at which it is fatal and should ring in the alarm
#define RESTART_GPIO 5
#define SHUTD_GPIO 6
#define DEBOUNCE 200
// Threaded loop function definitions
void* ldr_loop(void* argc);
void* display_loop(void* argc);
void* temp_loop(void* argc);
void* alert_loop(void* argc);
void* co2_loop(void* argc);
void* co_loop(void* argc);
void* uplink_loop(void* argc);
void* interrupt_watch(void* argc); //this one responds to the incoming system signals
// int setup_interrupts(void* on_restart, void* on_shutd); //this setups the interrupts from the buttons using wiringpi isr
// interrupt handlers
typedef struct {
  float co2_ppm;
  float temp_celcius;
  float light_cent;
  float co_ppm;
  char err[128];
}ambience; //this structure represents the conditions collectively that are thread safe
ambience ambientnow = {.co2_ppm=0, .temp_celcius=0, .light_cent=0, .co_ppm=0}; //presetting the values in the structure
pthread_t tids[7];
pthread_mutex_t lock;
static sigset_t   signal_mask;  /* signals to block         */
// More often than not , yo find the need to check the readings from the ADS - this can give you the readings correctly
int main_test(int argc, char const *argv[]) {
  // co_loop(NULL);
  // float readings[4];
  // while (1) {
  //   // if (ads115_read_all_channels(0x48,readings)!=0) {
  //   //   perror("This was not well read by ads channel");
  //   // }
  //   // printf("%.4f\t\t%.4f\t\t%.4f\t\t%.4f\n", readings[0],readings[1],readings[2], readings[3]);
  //   // float channel;
  //   // if (ads115_read_channel(0x48,2,GAIN_TWO,DR_128, &channel)!=0) {
  //   //   perror("Error reading a single channel");
  //   // }
  //   // printf("Channel 2: %.3f\n", channel);
  //   // lm35Result result;
  //   // if(airtemp_now(LM35_CHANNEL,CELCIUS, &result)!=0){
  //   //   perror("Error reading the temperature..");
  //   // }
  //   // printf("Temp: %.3f\n", result.temp);
  //
  //   sleep(2);
  // }
  wiringPiSetupGpio();
  int npn_invert =1;
  heater_full_power(MQ7_HEATER_GPIO,npn_invert );
  printf("Heater started in full power mode..\n");
  sleep(5);
  heater_power(0.10,MQ7_HEATER_GPIO,npn_invert);
  printf("Heater now in partial power mode..\n");
  sleep(30);
  printf("Heater now being turned off\n");
  heater_off(MQ7_HEATER_GPIO, npn_invert);
  return 0;
}
int main(int argc, char const *argv[]) {
  int ok =0;
  wiringPiSetupGpio();
  // instantiating the lock here
  sigemptyset (&signal_mask);
  sigaddset (&signal_mask, SIGINT);
  sigaddset (&signal_mask, SIGTERM);
  /*We dont want any of the threads including this one to handle any of the terminal signals
  We woudl want only one thread only to handle the system signals*/
  if(pthread_sigmask (SIG_BLOCK, &signal_mask, NULL)!=0){
    perror("sensing/main:failed to set signal block configuration");
    exit(EXIT_FAILURE);
  }
  if (pthread_mutex_init(&lock, NULL)!=0) {
    printf("Error starting a new thread, exiting application\n");
    exit(EXIT_FAILURE);
  }
  if (pthread_create(&tids[0], NULL,&ldr_loop, NULL)!=0) {
    printf("Failed to start activity thread\n");
    exit(EXIT_FAILURE);
  }
  if (pthread_create(&tids[1], NULL,&display_loop, NULL)!=0) {
    printf("Failed to start activity thread\n");
    exit(EXIT_FAILURE);
  }
  if (pthread_create(&tids[2], NULL,&temp_loop, NULL)!=0) {
    printf("Failed to start activity thread\n");
    exit(EXIT_FAILURE);
  }
  if (pthread_create(&tids[3], NULL,&co2_loop, NULL)!=0) {
    printf("Failed to start activity thread\n");
    exit(EXIT_FAILURE);
  }
  if (pthread_create(&tids[4], NULL,&co_loop, NULL)!=0) {
    printf("Failed to start activity thread\n");
    exit(EXIT_FAILURE);
  }
  if (pthread_create(&tids[5], NULL,&uplink_loop, NULL)!=0) {
    printf("Failed to start activity thread\n");
    exit(EXIT_FAILURE);
  }
  if (pthread_create(&tids[6], NULL,&interrupt_watch, NULL)!=0) {
    printf("Failed to start activity thread\n");
    exit(EXIT_FAILURE);
  }
  size_t i;
  int array_sz = sizeof(tids)/sizeof(pthread_t);
  for (i = 0; i < array_sz; i++) {
    pthread_join(tids[i], NULL);
  }
  lcd_clear();
  mq7_shutdown(MQ7_HEATER_GPIO,1);
  clear_all_alerts();
  pthread_mutex_destroy(&lock);
  exit(EXIT_SUCCESS);
}
void* interrupt_watch(void* argc){
  int sig_caught;    /* signal caught       */
  size_t i;
  int array_sz = sizeof(tids)/sizeof(pthread_t);
  if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)!=0) {
    perror("sensing/ co_loop:failed to set cancel state");
    exit(EXIT_FAILURE);
  }
  if(sigwait (&signal_mask, &sig_caught)!=0){
    perror("sensing/main: error setting up the signal listeners");
  }
  switch (sig_caught) {
    case SIGINT:
    case SIGTERM:
    case SIGKILL:
      for (i = 0; i < array_sz; i++) {
        pthread_cancel(tids[i]);
      }
      break;
    default:
      printf("Caught the signal but not the one expected\n");
  }
}
void* co_loop(void* argc){
  if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)!=0) {
    perror("sensing/ co_loop:failed to set cancel state");
    exit(EXIT_FAILURE);
  }
  int npn_invert = 1;
  mq7result result;
  while (1) {
    heater_full_power(MQ7_HEATER_GPIO,npn_invert);//full power heater for 60 secs
    usleep(60*SECSTOMICROSECS);
    // we know from rpi pin voltage it is 5.11 V so 28% makes 1.43V
    // partial power heater for 90 secs
    heater_power(0.10, MQ7_HEATER_GPIO, npn_invert);
    usleep(90*SECSTOMICROSECS);
    pthread_mutex_lock(&lock);
    if (ppm_co(MQ7_CHANNEL, &result)!=0){
      perror("sensing/co_loop: failed to read co content");
    }
    ambientnow.co_ppm=result.co_ppm; //psuhing to a shared structure
    // printf("CO : %.2f\n", result.co_ppm);
    pthread_mutex_unlock(&lock);
    heater_full_power(MQ7_HEATER_GPIO, npn_invert);
    // sleep only for 2 seconds before the next reading
    usleep(2*SECSTOMICROSECS);
  }
  pthread_exit(0);
}
void* co2_loop(void* argc){
  if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)!=0) {
    perror("sensing/ co_loop:failed to set cancel state");
    exit(EXIT_FAILURE);
  }
  setup_alert(BLUE_GPIO,RED_GPIO,BUZZ_GPIO);
  mq135Result result;
  int ok =0;
  while (1) {
    pthread_mutex_lock(&lock);
    if(ppm_co2(MQ135_CHANNEL,0,&result)!=0){
      perror("sensing: failed to measure co2 content");
    }
    ambientnow.co2_ppm=result.ppmCo2;
    if (ambientnow.co2_ppm < Co2_LVL_WARN) {
      alert(&ok,0,0);
    }
    else if (ambientnow.co2_ppm >= Co2_LVL_WARN && ambientnow.co2_ppm < Co2_LVL_ALARM){
      alert(&ok,1,0);
    }
    else {alert(&ok,2,0);}
    pthread_mutex_unlock(&lock);
    usleep(5*SECSTOMICROSECS); // we need co2 to be measured every 2 seconds
  }
  pthread_exit(0);
}
void* temp_loop(void* argc){
  if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)!=0) {
    perror("sensing/ co_loop:failed to set cancel state");
    exit(EXIT_FAILURE);
  }
  lm35Result result;
  while (1) {
    pthread_mutex_lock(&lock);
    if(airtemp_now(LM35_CHANNEL,CELCIUS, &result)!=0){
      perror("Error reading the temperature..");
    }
    ambientnow.temp_celcius = result.temp;
    pthread_mutex_unlock(&lock);
    usleep(10*SECSTOMICROSECS);
  }
  pthread_exit(0);
}
void* display_loop(void* argc){
  if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)!=0) {
    perror("sensing/ co_loop:failed to set cancel state");
    exit(EXIT_FAILURE);
  }
  char message[80];
  setup_lcd_4bitmode(LCD_ROWS,LCD_COLS,LCD_RS,LCD_E,LCD_D0,LCD_D1,LCD_D2,LCD_D3);
  while (1) {
    pthread_mutex_lock(&lock);
    display_readings(ambientnow.temp_celcius, ambientnow.light_cent*100,ambientnow.co2_ppm,ambientnow.co_ppm);
    pthread_mutex_unlock(&lock);
    usleep(2*SECSTOMICROSECS); //refresh the display every 2 second
  }
  lcd_clear();
  pthread_exit(0);
}
void* ldr_loop(void* argc){
  if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)!=0) {
    perror("sensing/ co_loop:failed to set cancel state");
    exit(EXIT_FAILURE);
  }
  ldrResult result;
  while (1) {
    // printf("%.3f\n",result.volts);
    pthread_mutex_lock(&lock);
    if (light_percent(LDR_CHANNEL, BRIGHT_VOLTS ,DARK_VOLTS,&result)!=0){
      perror("ldr loop: error reading the light conditions");
    }
    ambientnow.light_cent = result.light;
    pthread_mutex_unlock(&lock);
    usleep(5*SECSTOMICROSECS);
  }
  pthread_exit(0);
}

void* uplink_loop(void* argc){
  if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)!=0) {
    perror("sensing/ co_loop:failed to set cancel state");
    exit(EXIT_FAILURE);
  }
  int isAuth =device_authorize(); // << cannot get this inside the loop - we would be wasting server calls that way
  
  while (1) {
    if (isAuth<0) { break;} //<< if the device is not authorised we would not allow any furhter uplinking pings
    pthread_mutex_lock(&lock);
    if(device_ping(ambientnow.temp_celcius, ambientnow.light_cent, \
      ambientnow.co2_ppm, ambientnow.co_ppm)<0){
      fprintf(stderr, "Error uploading the conditions to the cloud\n");
      // << ideally this should spit the error in the error.log but for now we are just hoping this is working
    }
    pthread_mutex_unlock(&lock);
    usleep(3*SECSTOMICROSECS);
  }
  pthread_exit(0);
}
