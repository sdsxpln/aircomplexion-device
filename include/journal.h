#include <stdio.h>
#include<string.h>

#ifndef ADC_H
#define ADC_H
#define JOURNAL_FPATH "/home/pi/src/aircomplexion-device/etc/device.journal"
int journal_append(char* message);
int journal_exception(char* errMessage);
int journal_flush();
#endif
