#ifndef UPLINK_H
#define UPLINK_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct MemoryStruct {
  char *memory;
  size_t size;
};
/*A characteristic of the function that represents the procedure that would
inturn read the key and value to a string - since json has strings , number and
boolean values there are those corresponding fnctions*/
typedef char* (*jsonify)(char*, char*);
/*this represents a single field in the json object, An array of such structure
woudl represent the entire json object to be serialized */
typedef struct structkeyvaluepair{
  char* key;
  char* strValue ;
  jsonify Jsonify;
}KeyValuePair;
/*this gives the details of the device being registered on the system
once the device is registered it would parse the uuid into this and send it back*/
typedef struct structdevicedetails{
  char* location;
  char* type;
  char* duty;
  char* uuid;
}DeviceDetails;
// this gives the rel url ahead of the base url to get the list of devices
#define DEVICES_URL "api/uplink/devices/"
/* functions to jsoify a single field in the given key value pair
key         : is the name of the key of the field
value       : is the value of the field under that key*/
char* jsonify_strfield(char* key, char* value);
char* jsonify_numfield(char* key, char* value);
char* jsonify_boolfield(char* key, char* value);
/*Serializes the KeyValuePair[] to a json string that can be sent over the curl requests
payload         : represents the object that would be serialized
fields          : number of fields in the payload, also the number of items in the KeyValuePair[]
ok              : the status of the result
returns the json string result of the entire serialized object
Will not consider fields that are empty so expect an empty string if the key of the fields are invalid*/
char* json_serialize(KeyValuePair payload[], unsigned int fields, int* ok);
long url_post(char* url, char* payload, char** content , long* response_code, int* ok);
/*since content is double indirection it means it is output  - light bulb moment for me :)*/
long url_get(char* url,char** content,long* response,int* ok);
int is_device_registered(char* baseUrl,  char* uuid);
#endif
