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
void test_register_new_device(){
  DeviceDetails details={"Hinjawadi Phase1","RPi3B+","Measuring the ambient conditions"};
  int result =register_device(&details, baseUrl);
  printf("%d : Return code from the function\n", result);
}
/*Finds and splits a string into 2 sections on the first occurence of the token
This will not find multiple token occurences and as such would need a better function
But for now this forms the basis of de-serialize the strings we receive from http response
toSplit       :this is the string we intend to search and explode
token           :tsubstring we are looking to demarkate and break the string
left            :left side of the split string
right           :right side of the split string
rightClip       :right side of the split string can be clipped , if entire string is desired issue 0
Returns 0 for no errors and negative for errors
*/
int split_string_on_token(const char* toSplit, const char* token, char** left, char** right, size_t rightClip){
  // char* subStr = "\"uuid\": \"";
  size_t tokenSz  = strlen(token); //we are going to proceed as this much in each
  if(tokenSz==0){return -1;}
  char* target = toSplit;
  char* clippedResult = malloc(0); //this is the final result
  while (*toSplit != '\0') {
    char* strSection  = malloc(tokenSz+1);
    memset(strSection,0,strlen(strSection)+1);
    memcpy(strSection,target, tokenSz);
    printf("Substring being compared\n%s\n", strSection);
    if(0==strcmp(strSection,token)){
      printf("Substring found\n");
      target += tokenSz;
      // now knowing that the uuid is only 36 chracters long
      clippedResult = realloc(clippedResult,rightClip);
      memset(clippedResult,0,strlen(clippedResult));
      memcpy(clippedResult,target,rightClip);
      printf("Clipped result: %s\n", clippedResult);
      break;
    }
    target++;
    free(strSection);
  }
}
void test_post_requests(){
  char url[strlen(baseUrl)+strlen("api/uplink/devices/")+2];
  sprintf(url, "%s%s",baseUrl,"api/uplink/devices/");
  DeviceDetails dd={"Hinjawadi Phase1","RPi3B+","Measuring the ambient conditions"};
  KeyValuePair payload[] ={
    {"location",dd.location, jsonify_strfield},
    {"type",dd.type, jsonify_strfield},
    {"duty",dd.duty, jsonify_strfield},
  };
  int ok =0;
  char* jsonPayload  = json_serialize(payload, 3, &ok);
  char* content = malloc(0);
  long rCode = 0;
  ok =0;
  assert(strcmp(jsonPayload, "")!=0);
  assert(jsonPayload !=NULL);
  printf("We are about to hit the url :\n%s\n", url);
  long bytesRecv= url_post(url, jsonPayload, &content , &rCode, &ok);
  // we need to then find out the content that we have received
  assert(rCode==200);
  assert(strcmp(content, "")!=0);
  printf("%s\n",content);
  // lets see if can really de-serialize the content from th received

}
int main(int argc, char const *argv[]) {
  test_post_requests();
  return 0;
}
