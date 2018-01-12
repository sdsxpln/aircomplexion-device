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
#include "ldr.h"
#include "display.h"
#include "lm35.h"
#include "alerts.h"
#include "mq135.h"
#include "adc.h"
#include "mq7.h"
#include "uplink.h"

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
pthread_t tids[5];
pthread_mutex_t lock;
static sigset_t   signal_mask;  /* signals to block         */
// More often than not , yo find the need to check the readings from the ADS - this can give you the readings correctly
void main_archive(){
  int ok =0;
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
  if (pthread_create(&tids[1], NULL,&temp_loop, NULL)!=0) {
    printf("Failed to start activity thread\n");
    exit(EXIT_FAILURE);
  }
  if (pthread_create(&tids[2], NULL,&co2_loop, NULL)!=0) {
    printf("Failed to start activity thread\n");
    exit(EXIT_FAILURE);
  }
  // if (pthread_create(&tids[3], NULL,&co_loop, NULL)!=0) {
  //   printf("Failed to start activity thread\n");
  //   exit(EXIT_FAILURE);
  // }
  if (pthread_create(&tids[3], NULL,&display_loop, NULL)!=0) {
    printf("Failed to start activity thread\n");
    exit(EXIT_FAILURE);
  }
  if (pthread_create(&tids[4], NULL,&interrupt_watch, NULL)!=0) {
    printf("Failed to start activity thread\n");
    exit(EXIT_FAILURE);
  }
  // if (pthread_create(&tids[5], NULL,&uplink_loop, NULL)!=0) {
  //   printf("Failed to start activity thread\n");
  //   exit(EXIT_FAILURE);
  // }
  if(pthread_sigmask (SIG_UNBLOCK, &signal_mask, NULL)!=0){
    perror("sensing/main:failed to set signal block configuration");
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
}

pthread_t  pool[2]; //<<< all the threads go in here ..
int poolSz  = sizeof(pool)/sizeof(pthread_t);
/*this is run whn you have signal incoming from the system  - this would help in evicting all the threads*/
static void cleanup (int sig, siginfo_t *siginfo, void *context){
  size_t i =0;
  for (i = 0; i < poolSz; i++) {
    pthread_cancel(pool[i]);
  }
}
int main(int argc, char const *argv[]) {
  /*We are instructing the main thread to setup actions tht are triggered on the signal receipt
  this helps in cleaning up and the activities*/
  struct sigaction act;
  memset (&act, '\0', sizeof(act));
  act.sa_sigaction = &cleanup;
  /* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
  act.sa_flags = SA_SIGINFO;
  if (sigaction(SIGINT, &act, NULL) < 0) {perror ("sigaction");exit(1);}
  if (sigaction(SIGTERM, &act, NULL) < 0) {perror ("sigaction");exit(1);}
  if (sigaction(SIGHUP, &act, NULL) < 0) {perror ("sigaction");exit(1);}
  /*it SIGKILL, SIGSTOP cannot be blocked , ignored or captured, this would lead to sigaction fault
  refer here https://stackoverflow.com/questions/18088924/signal-handler-function-catches-sigkill-the-first-3-times*/
  // if (sigaction(SIGKILL, &act, NULL) < 0) {perror ("sigaction");exit(1);}
  /*We then make a mask which all the worker threads would be inheriting
  We want all the worker threads to basically block the 3 signasl from the system
  By design then the main thread the only contender to handle the kill signals*/
  sigset_t   signal_mask;
  sigemptyset (&signal_mask);
  sigaddset (&signal_mask, SIGINT);
  sigaddset (&signal_mask, SIGTERM);
  if(pthread_sigmask (SIG_BLOCK, &signal_mask, NULL)!=0){
    fprintf(stderr, "Failed to set signal mask\n");
    exit(1);
  }
  if (pthread_create(&pool[0], NULL,&co2_loop, NULL)!=0) {
    printf("Failed start co2 loop on a thread\n");
    exit(1);
  }
  if (pthread_create(&pool[1], NULL,&display_loop, NULL)!=0) {
    printf("Failed start co2 loop on a thread\n");
    exit(1);
  }
  /*once all the threads are spawned we want the main thread to be back again to it default bahaviour*/
  if(pthread_sigmask (SIG_UNBLOCK, &signal_mask, NULL)!=0){
    fprintf(stderr, "Failed to set signal mask\n");
    exit(1);
  }
  size_t i;
  for (i = 0; i < poolSz; i++) {
    pthread_join(pool[i], NULL);
  }
  fprintf(stderr, "All the sensing loops have exited\n");
  // lcd_clear(); //<<turn this on when you have the display loop else this would give segmentation fault
  clear_all_alerts();
  lcd_clear();
  pthread_mutex_destroy(&lock);
  exit(EXIT_SUCCESS);
}
void* interrupt_watch(void* argc){
  int sig_caught;    /* signal caught       */
  size_t i;
  if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)!=0) {
    perror("sensing/ co_loop:failed to set cancel state");
    exit(EXIT_FAILURE);
  }
  printf("We are now going to wait for system signal\n");
  if(sigwait (&signal_mask, &sig_caught)!=0){
    perror("sensing/main: error setting up the signal listeners");
  }
  printf("We have received some signal\n");
  switch (sig_caught) {
    case SIGINT:
    case SIGTERM:
    case SIGKILL:
      printf("We have positively caught the kill signal \n");
      for (i = 0; i < poolSz; i++) {
        pthread_cancel(pool[i]);
      }
      break;
    default:
      printf("Caught the signal but not the one expected\n");
  }
  pthread_exit(0);
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
  pthread_t self = pthread_self();
  if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)!=0) {
    fprintf(stderr, "Could not set the cancel state of thread : co2 loop\n");
    pthread_exit(&self);
  }
  setup_alert(BLUE_GPIO,RED_GPIO,BUZZ_GPIO);
  mq135Result result;
  int ok =0;
  while (1) {
    pthread_mutex_lock(&lock);
    if(ppm_co2(MQ135_CHANNEL,&result)!=0){
      perror("sensing: failed to measure co2 content");
    }
    ambientnow.co2_ppm=result.ppmCo2;
    // printf("%.2f\t\t%.2f\n",result.volts, result.ppmCo2);
    if (ambientnow.co2_ppm < Co2_LVL_WARN) {alert(&ok,0,0);}
    else if (ambientnow.co2_ppm >= Co2_LVL_WARN && ambientnow.co2_ppm < Co2_LVL_ALARM){
      alert(&ok,1,0);
    }
    else {alert(&ok,2,0);}
    pthread_mutex_unlock(&lock);
    usleep(2*SECSTOMICROSECS); // we need co2 to be measured every 2 seconds
  }
  pthread_exit(&self);
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
  // char message[80];
  if(setup_lcd_4bitmode(LCD_ROWS,LCD_COLS,LCD_RS,LCD_E,LCD_D0,LCD_D1,LCD_D2,LCD_D3)<0){
    fprintf(stderr, "Error setting up the the LCD \n");
    pthread_exit(0);
  };
  lcd_message("We are about to start readings");
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
