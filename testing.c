#include<stdio.h>
#include<stdlib.h>
#include"./uplink/uplink.h"
#include<assert.h>

char* testUuids[]={
  "b0d90dee-b228-42b3-aa7b-e7e9850549a8",
  "5e275f6b-3657-4262-9f87-202fabaab0d4",
  "b370eaef-1544-4178-9ee8-d6fba35f5dd4",
  "6410d1b0-c62e-420d-bfcd-fed87a279cb3",
  "0522b138-e327-4177-b5a8-33ccaff8613f",
  "af25367b-7249-4ae5-9832-216445bbce9d",
  "f54990cd-1541-4418-adad-e399efe84099",
  "caedd6a0-f9e0-41e5-8b3f-2af757f7ec60",
  "36c38d62-43df-4584-b27d-1164dfc11a8b",
  "40ed61c0-53fc-4f1e-bac9-21ed10abbcda",
  "56e0dd78-085b-433e-a75a-c154642aaa12",
  "18fb811d-1f93-435a-b581-39f93ae99ff0",
  "a99745ad-9f17-45e6-83ac-c00d5e06b8c4"
};

/*We had observed problems in url:
When the device uuid which were present in the databse were fired first url_get gave an error on the second hit saying Segmentatio fault.
This is to figure out what excatly is the problem that bugging url_get*/
// void test_url_get(){
//   size_t i;
//   size_t count  = (size_t)(sizeof(testUuids)/sizeof(testUuids[0]));
//   for (i = 0; i < count; i++) {
//     char url[strlen(baseUrl)+strlen("api/uplink/devices/")+strlen(testUuids[i])+2];
//     sprintf(url, "%s%s%s/",baseUrl,"api/uplink/devices/",testUuids[i]);
//     int ok =0; long response =0; char* content = malloc(0);
//     long bytes_received =url_get(url,&content,&response,&ok);
//     if (strcmp(content, "")!=0) {
//       printf("%s\n", content);
//     }
//     else{printf("Device not found\n");}
//   }
//   return;
// }
// void test_uplinking(){
//   // here now we have an interface where we can test if the device is registered
//   size_t i;
//   size_t count  = (size_t)(sizeof(testUuids)/sizeof(testUuids[0]));
//   // printf("%d: the number of urls we have to assess\n",count );
//   for (i = 0; i < count; i++) {
//     int result =is_device_registered(baseUrl, testUuids[i]);
//     printf("%s\t\t", testUuids[i]);
//     if(result ==0){printf("Not Found\n");}
//     else{printf("Found\n");}
//   }
//   return;
// }
// // void test_post_requests(){
// //   char url[strlen(baseUrl)+strlen("api/uplink/devices/")+1];
// //   sprintf(url, "%s%s",baseUrl,"api/uplink/devices/");
// //   /*forming the json payload for post
// //   Please be careful  here : if the device details is formed using the initilizer
// //   then it leads to a segentation fault since it has 4 fields and only
// //   3 are initilized*/
// //   DeviceDetails dd;
// //   dd.location ="Hinjawadi Phase1, Pune 57";
// //   dd.type = "RPi3B+";
// //   dd.duty = "Measuring ambient conditions";
// //   KeyValuePair payload[] ={
// //     {"type",dd.type, jsonify_strfield},
// //     {"duty",dd.duty, jsonify_strfield},
// //     {"location",dd.location, jsonify_strfield},
// //   };
// //   int ok =0;
// //   char* jsonPayload  = json_serialize(payload, 3, &ok);
// //   printf("Json payload formed \n%s\n",jsonPayload);
// //   assert(ok==0);
// //   assert(strcmp(jsonPayload, "")!=0);
// //   assert(jsonPayload !=NULL);
// //   char* content = malloc(0);
// //   long rCode = 0;
// //   ok =0;
// //   printf("We are about to hit the url :\n%s\n", url);
// //   long bytesRecv= url_post(url, jsonPayload, &content , &rCode, &ok);
// //   // we need to then find out the content that we have received
// //   assert(rCode==200);
// //   assert(strcmp(content, "")!=0);
// //   printf("%s\n",content);
// //   // lets see if can really de-serialize the content from th received
// //   // We know the uuid of the device is 36 characters long
// //   char * left = malloc(0);
// //   char* right = malloc(0);
// //   printf("%s\n", content);
// //   if (splitstr_token(content ,"\"uuid\": \"", &left, &right, 36) !=0) {
// //     dd.uuid = right;
// //     printf("UUID of the newly posted device \n%s\n", dd.uuid);
// //   }
// // }
// void test_string_split(){
//   DeviceDetails dd={"Hinjawadi Phase1","RPi3B+","Measuring the ambient conditions","0513ea9a-a8b3-4300-bd29-dbbca89cc781"};
//   KeyValuePair payload[] ={
//     {"type",dd.type, jsonify_strfield},
//     {"duty",dd.duty, jsonify_strfield},
//     {"uuid",dd.uuid, jsonify_strfield},
//     {"location",dd.location, jsonify_strfield},
//   };
//   int ok =0;
//   char* jsonPayload  = json_serialize(payload, 4, &ok);
//   char* left  = malloc(0);
//   char* right = malloc(0);
//   printf("%s\n", jsonPayload);
//   if (splitstr_token(jsonPayload ,"\"chutya\": \"", &left, &right, 36) !=0) {
//     printf("%s\n",left);
//     printf("%s\n",right);
//   }
//   else{printf("Failed to spot the token in the string\n");}
// }
// // NOTE: how to design a simple system that can give us a simple jsonification system
void test_to_json(){
  printf("%s\n","-----------New function call to to_json---------------");
  KeyValuePair payload[] ={
    {"type","RPi3B", jsonify_strfield},
    {"duty","Ambient conditions monitoring", jsonify_strfield},
    {"uuid","a99745ad-9f17-45e6-83ac-c00d5e06b8c4", jsonify_strfield},
    {"location","Kothrud Pune 38", jsonify_strfield},
  };
  // KeyValuePair payload_1[] ={
  //   {"type","RPi3B+", jsonify_strfield},
  //   {"duty","Ambient conditions monitoring", jsonify_strfield},
  //   {"uuid","a99745ad-9f17-45e6-83ac-c00d5e06b8c4", jsonify_strfield},
  //   {"location","Kothrud Pune 38, Near Shivaji Statue", jsonify_strfield},
  // };
  KeyValuePair invalidPayload [] ={
    {"","Kothrud Pune 38", jsonify_strfield},
    {"","RPi3B", jsonify_strfield},
    {"","Measuremen of ambient air conditions", jsonify_strfield},
    {"","Some uuid", jsonify_strfield},
  };
  char* json = (char*)malloc(sizeof(char)+1);
  memset(json, 0, strlen(json));
  int result=0;
  result  = to_json(payload,4,&json);
  printf("%s\n",json);
  free(json);
  // result  = to_json(payload_1,4,&json);
  // printf("%s\n",json);
  //
  // printf("And as for the empty pay load this is what we have received \n");
  // // json = (char*)malloc(1);
  // result  = to_json(emptyPayload,4,&json);
  // printf("%s\n",json);
  printf("And as for the invalid payload where keys are absent : \n");
  json = (char*)malloc(sizeof(char)+1);
  memset(json, 0, strlen(json));
  result  = to_json(invalidPayload,4,&json);
  printf("%s\n",json);
  free(json);
}
int main(int argc, char const *argv[]) {
  char* baseUrl  = "http://192.168.1.5:8038/";
  char* oldUUID  = "a99745ad-9f17-45e6-83ac-c00d5e06b8c4";
  char* uuid  = (char*)malloc(sizeof(char));
  KeyValuePair invalidPayload [] ={
    {"","Kothrud Pune 38", jsonify_strfield},
    {"","RPi3B", jsonify_strfield},
    {"","Measuremen of ambient air conditions", jsonify_strfield},
    {"","Some uuid", jsonify_strfield},
  };
  KeyValuePair payload[] ={
    {"type","RPi3B", jsonify_strfield},
    {"duty","Ambient conditions monitoring", jsonify_strfield},
    {"uuid","a99745ad-9f17-45e6-83ac-c00d5e06b8c4", jsonify_strfield},
    {"location","Kothrud Pune 38", jsonify_strfield},
  };
  if (register_device(payload,baseUrl, &uuid)==0) {
    printf("%s\n","Success in posting the device to the database");
    printf("%s\n", uuid);
    // testing if the device registry works with the uuid that we have
    if (is_device_registered(baseUrl,uuid)==1) {
      printf("%s\n","Found the device registered just a while back" );
    }
  }
  else{
    printf("%s\n", "Failed to find the device that was registered");
  }
  // if (is_device_registered(baseUrl, oldUUID)==0) {
  //   // Try to register device if the with the oldUUID there is nodevice
  //   printf("%s\n", "Device not found registered, movig ahead to register the device");
  //
  // }
  // // test_to_json();
  // // test_to_json();
  // // KeyValuePair* payload = malloc(sizeof(KeyValuePair)*4);
  // // char* json = (char*)malloc(sizeof(char));
  // // int result=0;
  // // result  = to_json(payload,4,&json);
  // // printf("%s\n",json);
  // // // Now trying to post the invalid json
  // // uuid  = malloc(sizeof(char));
  // // if (register_device(invalidPayload,baseUrl, &uuid)!=0) {
  // //   fprintf(stderr, "%s\n", "Failed test for register device .. kindly check again");
  // //   return -1;
  // // }
  // // printf("%s\n","Success in posting the device to the database");
  // // printf("%s\n", uuid);
  return 0;
}
