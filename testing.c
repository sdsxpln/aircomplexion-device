#include<stdio.h>
#include<stdlib.h>
#include"./uplink/uplink.h"


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
char* baseUrl  = "http://192.168.1.5:8038/";
/*We had observed problems in url:
When the device uuid which were present in the databse were fired first url_get gave an error on the second hit saying Segmentatio fault.
This is to figure out what excatly is the problem that bugging url_get*/
void test_url_get(){
  size_t i;
  size_t count  = (size_t)(sizeof(testUuids)/sizeof(testUuids[0]));
  for (i = 0; i < count; i++) {
    char url[strlen(baseUrl)+strlen("api/uplink/devices/")+strlen(testUuids[i])+2];
    sprintf(url, "%s%s%s/",baseUrl,"api/uplink/devices/",testUuids[i]);
    int ok =0; long response =0; char* content = malloc(0);
    long bytes_received =url_get(url,&content,&response,&ok);
    if (strcmp(content, "")!=0) {
      printf("%s\n", content);
    }
    else{printf("Device not found\n");}
  }
  return;
}

void test_uplinking(){
  // here now we have an interface where we can test if the device is registered
  size_t i;
  size_t count  = (size_t)(sizeof(testUuids)/sizeof(testUuids[0]));
  // printf("%d: the number of urls we have to assess\n",count );
  for (i = 0; i < count; i++) {
    int result =is_device_registered(baseUrl, testUuids[i]);
    printf("%s\t\t", testUuids[i]);
    if(result ==0){printf("Not Found\n");}
    else{printf("Found\n");}
  }
  return;
}

int main(int argc, char const *argv[]) {
  test_uplinking();
  return 0;
}
