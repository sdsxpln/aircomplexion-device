#include <stdio.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <unistd.h>
#include "adc.h"
/*
author    :kneerunjun
This was created to modulate the load from device sensing loop
this would control the working of analog to digital conversions for the raspberry pi
*/

PGA defGain=GAIN_ONE; //this is setting the default
DTRATE defDR = DR_128; //this is the default setting on the data rate

/*This gets the range  of each of the gain levels
gain      : this the enum choice the user selects */
float gain_range(PGA gain){
  float range = 4.096; //this is the default value
  switch (gain) {
    case GAIN_TWOTHIRDS:
      range= 6.144;break;
    case GAIN_ONE:
      range =4.096;break;
    case GAIN_TWO:
      range =2.048;break;
    case GAIN_FOUR:
      range= 1.024;break;
    case GAIN_EIGHT:
      range =0.512;break;
    case GAIN_SIXTEEN:
      range=0.256;break;
    default :
      range =4.096;break;
  }
  return range;
}
/*All this function essentially converts is the the enum to requisite int value*/
int samples_per_sec(DTRATE dr){
  switch (dr) {
    case DR_8:
      return 8;
    case DR_16:
      return 16;
    case DR_32:
      return 32;
    case DR_64:
      return 64;
    case DR_128:
      return 128;
    case DR_250:
      return 250;
    case DR_475:
      return 475;
    case DR_860:
      return 860;
    default:
      return 128;
  }
}
/*
This would read all the 4 channels with the default configurations
config[0]     : the register id that you need to write to
config[1]     : msb of the configuration
config[2]     : lsb of the configuration
slaveaddr     : depending on the hardware configuraion you can actually set this, default 0x48
error         : this is the success indicator < 0 : Error  ==0 is Success , -1 == device error , -2 : register error
*/

float read_device(uint8_t conf[], int addr , PGA gain, DTRATE dr ,int* error){
  int fd; //this is the device pointer.
  int sps=samples_per_sec(dr);// here we convert the bit value of te data rate to the actual data rate.
  const float VPS = gain_range(gain)/32767.0;
  int16_t val;
  uint8_t readBuffer[3] ;
  if ((fd = open("/dev/i2c-1", O_RDWR)) <0) {
    printf("Could not open device %d\n",fd );
    return -1;
  }
  if(ioctl(fd, I2C_SLAVE, addr) < 0){
    printf("Failed to connect to I2C slave\n");
    return -1;
  }
  if (write(fd, conf, 3)!=3) {
    perror("configuration write error:");
    printf("There was error writing to the configuration register\n");
    *error= -1;
    return 0;
  }
  do {
    if (read(fd, conf, 2)!=2) {
      printf("Could not read the register \n" );
      *error= -1;
      return 0;
    }
  } while(conf[0] & 0x80 ==0);
  usleep((1/(float)sps)*1000000+2000);
  readBuffer[0] = 0;
  if (write(fd, readBuffer,1)!=1) {
    *error= -2;
    return 0;
  }
  // and then we go ahead to read from the conversion register
  if (read(fd, readBuffer, 2)!=2) {
    printf("Error reading the conversion register\n");
    *error= -2;
    return 0;
  }
  val  = (readBuffer[0] <<8 | readBuffer[1]);
  if (val <0) {
    val =0.00;
  }
  close(fd);
  *error  =0;
  return val*VPS;
}
int ads115_read_all_channels(int slaveaddr,float* readings){
  /*
  slaveaddr     :this is the I2C slave address
  readings      :result of the readings on all the channels
  */
  uint8_t a1Config[3],a0Config[3], a2Config[3], a3Config[3];
  int err =0;
  // channel0 configuration
  a0Config[0]=1;
  a0Config[1]=0b11000011;
  a0Config[2]=0b10000011;
  // channel1 configuration
  float  volts  = read_device(a0Config, 0x48,defGain,defDR,&err);
  if (err!=0) {return err;}
  *(readings) = volts; //if it was clean read from the device
  a1Config[0]=1;
  a1Config[1]=0b11010011;
  a1Config[2]=0b10000011;
  // channel2 configuration
  volts  = read_device(a1Config, 0x48,defGain,defDR,&err);
  if (err!=0) {return err;}
  *(readings+1) = volts;//if it was a clean read
  a2Config[0]=1;
  a2Config[1]=0b11100011;
  a2Config[2]=0b10000011;
  volts  = read_device(a2Config, 0x48,defGain,defDR,&err);
  if (err!=0) {return err;}
  *(readings+2) = volts;
  // channel3 configuration
  a3Config[0]=1;
  a3Config[1]=0b11110011;
  a3Config[2]=0b10000011;
  volts  = read_device(a3Config, 0x48,defGain,defDR,&err);
  if (err==0) {return err;}
  *(readings+3) = volts;
  return 0;
}
/*this allows reading a single channel at a time with custom gain values
slaveaddr       : Address of the I2C slave
channel         : A0,1,2,3 numbers of the channels ranging from 0-4 to signify each of the channels marked on the ADC
gain            : programmable gain amplification
dr              : data rate to determine the samples per sec
ok              : 0 for sucess , -1 for error
*/
float ads115_read_channel(int slaveaddr, int channel, PGA gain, DTRATE dr,int* ok){
  uint8_t config[3], a1Config[3];
  float volts =0.00;
  channel = channel+ 4; //to offset the 4 comparator channels at the beginning
  config[0]=1; // since we would want to point to the config register
  config[1]=0b00000000;//MSB of the configuration register, it default without any value.
  config[1]=config[1] | 1 << 7; // OS starting single shot conversion mode
  config[1] = config[1] |channel<<4;
  config[1] = config[1] | gain << 1; //the pga in the byte is 11:9
  config[1] = config[1] | 1; //this is to set the mode to single shot power down
  config[2]=0b10000011; //LSB of the configuration register
  // here we can go ahead to adjust the data rate as requested
  // config[2]= config[2] | dr <<5;
  volts = read_device(config, slaveaddr,gain,dr,ok);
  if (*ok !=0) {
    // this is when we had a problem reading the device
    volts = 0.00;
  }
  return volts;
}
