#ifndef ADC_H
#define ADC_H
typedef enum {GAIN_TWOTHIRDS=0, GAIN_ONE=1, GAIN_TWO=2, GAIN_FOUR=3, GAIN_EIGHT=4, GAIN_SIXTEEN=5} PGA;
typedef enum {DR_8=0, DR_16=1, DR_32=2, DR_64=3, DR_128=4,DR_250=5, DR_475=6, DR_860=7}DTRATE;
int ads115_read_all_channels(int slaveaddr,float* readings);
float ads115_read_channel(int slaveaddr, int channel, PGA gain, DTRATE dr,int* ok);
#endif
