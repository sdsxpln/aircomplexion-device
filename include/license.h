#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef LICENSE_H
#define LICENSE_H


#define LIC_FILE_PATH "/home/pi/src/aircomplexion-device/etc/device.license"
#define MAX_BUFF  1024

int get_device_uuid(char** uuid);
int get_device_loc(char** loc);
int get_device_owner(char** email);
int get_device_duty(char** duty);
int get_device_type(char** type);
int get_license_server(char** baseUrl);

int update_device_uuid(char* uuid);
int update_device_loc(char* loc);
int update_device_owner(char* email);
int update_device_duty(char* duty);
int update_device_type(char* type);

#endif
