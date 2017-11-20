#include<stdio.h>
#include <unistd.h>
#include "./mq7/mq7.h"
#include <pthread.h>
#include "./display/display.h"
pthread_mutex_t lock;
// This is where the threads deposit their values
typedef struct {
  float co_ppm; //this is the value of co in the atmosphere
  float co_volts;
  float co_kohms;
}conditions;
conditions myconditions = {.co_ppm=0,.co_volts=0,.co_kohms=0};
#define RS 9
#define E 11
#define D0 14
#define D1 15
#define D2 18
#define D3 23
#define TOMICROSECS 1000000
pthread_t tids[2];
void* mq7_loop(void* arg){
  size_t i=0;
  int gpio =13 ; //this is the pin at which voltage control is attached
  int ok =0;
  int channel =3;
  int npn_invert =1;
  for (i = 0; i < 20; i++) {
    heater_full_power(gpio, npn_invert);
    usleep(60*TOMICROSECS);
    heater_power(0.28, gpio,npn_invert); // we know from rpi pin voltage it is 5.11 V so 28% makes 1.43V - and remember the npn is now a invertor
    usleep(90*TOMICROSECS);
    // heater_full_power(gpio,npn_invert);
    mq7result result =ppm_co(&ok ,channel); //this is where you get the reading from sensor
    heater_off(gpio,npn_invert);
    pthread_mutex_lock(&lock);
    myconditions.co_ppm=  result.co_ppm;
    myconditions.co_volts=result.volts;
    myconditions.co_kohms=result.sensor_kohms;
    // We also attempt to read from the shared structure
    printf("%.2f\t\t%.2f\t\t%.2f\n", myconditions.co_volts, myconditions.co_kohms, myconditions.co_ppm);
    pthread_mutex_unlock(&lock);
    usleep(3*TOMICROSECS);
  }
  printf("The sensing loop has now maxed out , we are exiting\n");
}

void* display_loop(void* arg){
  char disp[100]="Sensor priming.."; //there is  a problem when we declare this as char*
  setup_lcd_4bitmode(2,16,RS,E,D0,D1,D2,D3);
  while(1) {
    pthread_mutex_lock(&lock);
    if (myconditions.co_ppm!=0) {
      sprintf(disp,"Co ppm: %.3f", myconditions.co_ppm);
    }
    lcd_message(disp);
    usleep(2*TOMICROSECS);
    pthread_mutex_unlock(&lock);
    usleep(3*TOMICROSECS); //we try to refresh the display every 2 seconds
  }
}
int main(int argc, char const *argv[]) {
  // here we make this into a multi threaded operation
  if (pthread_mutex_init(&lock, NULL)!=0) {
    printf("Error starting a new thread, exiting application\n");
    return 1;
  }
  if (pthread_create(&tids[0], NULL,&mq7_loop, NULL)!=0) {
    printf("There was a problem initiating thread in the process\n");
    return 1;
  }
  if (pthread_create(&tids[1], NULL, &display_loop, NULL)!=0) {
    printf("There was a problem initiating display thread\n");
    return 1;
  }
  size_t i =0;
  // to join threads remember that the display thread is a infinite loop
  // we just need to wait on the sensing loop and then call to destroy the display loop
  pthread_join(tids[0], NULL);
  // this is where the display looop needs to be called off
  if (pthread_cancel(tids[1])!=0) {
    printf("There was a problem quitting the display loop\n");
  }
  printf("All the threads have returned, now destroying the mutex locks\n");
  pthread_mutex_destroy(&lock);
  return 0;
}
