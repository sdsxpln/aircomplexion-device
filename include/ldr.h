#ifndef LDR_H
#define LDR_H
typedef struct {
  float volts;
  float light;
  float max_min[2];
  int ok;
}ldrResult;
int light_percent(int adschn, float voltbright, float voltdark, ldrResult* result);
#endif
