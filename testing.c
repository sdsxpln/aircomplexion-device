#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include <sys/time.h>
#include<string.h>
#include"journal.h"
#include "adc.h"

long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}
int main(int argc, char const *argv[]) {
  /*here we intend to test what happens when you try to measure voltages on the ADS channel greater than Vcc*/
  int result  = -1, channel =1;
  float volts = 0.0;
  while (1) {
    if(result =  ads115_read_channel(0x48,channel,0,4,&volts) <0 ){
      fprintf(stderr, "We have encounered an error reading the ADS channel\n");
      exit(1);
    }
    printf("%.2f\n",volts);
    sleep(2);
  }
  exit(0);
}
