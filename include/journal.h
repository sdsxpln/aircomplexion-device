#include <stdio.h>
#include<string.h>
#include<time.h>
#ifndef ADC_H
#define ADC_H
#define JOURNAL_FPATH "/home/pi/src/aircomplexion-device/etc/device.journal"
// #define JRNL
int journal_debug(char* message, char* sender);
int journal_exception(char* errMessage, char* sender);
int journal_flush();
#endif
