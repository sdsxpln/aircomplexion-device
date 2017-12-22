#include "uplink.h"
// you can reference some example from here
// post data example https://gist.github.com/jay/2a6c54cc6da442489561
// size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
//     data->append((char*) ptr, size * nmemb);
//     return size * nmemb;
// }
/*this function helps to send a GET request to the url */
/*This defines the KeyValuePair of json values that needs to be read back into a
json string before being sent across to the server. The client can now still put
in values in a dictionary
Want to know how to effectively define structures ?
https://www.programiz.com/c-programming/c-structures
Want to know how we can store function pointers in structures ?
http://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)FunctionPointers.html*/
char* jsonify_strfield(char* key, char* value){
    //this assumes the field value is char* type and thus converts the same
    char* new_result= malloc(strlen(key)+strlen(value)+10); // initiating to a proper size
    if(!new_result){
      fprintf(stderr, "Out of memory , failed jsonification \n");
      return "";
    }
    // notice that there is , mark after every field
    sprintf(new_result ,"\"%s\":\"%s\",",key, value);
    return new_result;
  }
char* jsonify_numfield(char* key, char* value){
    //this assumes the field value is char* type and thus converts the same
    char* new_result= malloc(strlen(key)+strlen(value)+10); // initiating to a proper size
    if(!new_result){
      fprintf(stderr, "Out of memory , failed jsonification \n");
      return "";
    }
    // notice that there is , mark after every field
    sprintf(new_result ,"\"%s\":%s,",key, value);
    return new_result;
  }
char* jsonify_boolfield(char* key, char* value){
  //this assumes the field value is char* type and thus converts the same
  char* new_result= malloc(strlen(key)+strlen(value)+10); // initiating to a proper size
  if(!new_result){
    fprintf(stderr, "Out of memory , failed jsonification \n");
    return "";
  }
  // notice that there is , mark after every field
  sprintf(new_result ,"\"%s\":%s,",key, value);
  return new_result;
}
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp){
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
  // want to read more about realloc ?
  // http://www.geeksforgeeks.org/g-fact-66/
  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0; // havent still discovered whats this!
  return realsize;
}
/*this helps in sending the data to the url as payload
url   :this is the post url
paylod : this is the xml/text data that is to be uploaded
reference :https://curl.haxx.se/libcurl/c/http-post.html
url       : full url which is to be hit
payload   : is the json string payload thats needed to ride the request
content   : response text from the server
response  : response code from the server
ok        : overall function response code
references/notes
// curl -d "param1=value1&param2=value2" -X POST http://localhost:3000/data
default bahaviour of CURL is to give CURLE_OK despite non 200 OK return codes
this mandates the curl to emit an error if thhe error from the server is returned
*/
long url_post(char* url, char* payload, char** content , long* response_code, int* ok){
  ok = 0;
  CURL* curl;
  CURLcode response = CURLE_OK;
  long bytesRecv =0;
  /*loading up the memory*/
  struct MemoryStruct chunk;
  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
  chunk.size = 0;
  memset(*content,0,strlen(*content)); // prepping the content pointer
  char error_buf[CURL_ERROR_SIZE];
  /*init the curl variables*/
  curl = curl_easy_init();
  if(!curl){
    // is the case when we could not init the curl pointer itself
    perror("failed to instantiate the CURL client");
    *ok =-1; //content is ""
    goto cleanup;
  }
  /*setting options*/
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L); //<= this is important, but not obvious
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
  /*actually performing the curl request*/
  response =curl_easy_perform(curl);
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, response_code);
  if (response!=CURLE_OK) {
    fprintf(stderr, "%s\n",error_buf);
    /*error string being copied to the content*/
    *content = realloc(*content,strlen(error_buf)+1);
    memcpy(*content,error_buf, strlen(error_buf));
    bytesRecv = strlen(error_buf);
    goto cleanup;
  }
  else{
    /*case when we have 200 Ok from the servr*/
    if(*response_code ==200 || *response_code ==201){
      if(0!=strcmp(chunk.memory, "null")){
        *content = realloc(*content, strlen(chunk.memory)+1);
        memcpy(*content,chunk.memory, strlen(chunk.memory));
        bytesRecv = chunk.size;
      }
    }
  }
  // printf("CURL POST success , status code %ld\n", response_code);
  // need to check if the response is in structure
  // printf("We have received %d bytes in the server response \n", chunk.size);
  // printf("%s\n", chunk.memory);
  cleanup:
    curl_easy_cleanup(curl);
    // curl_global_cleanup();
  return bytesRecv;
}
long url_get(char* url, char** content,long* response,int* ok){
  *ok  = 0; //at the onset everything is ok
  CURL *curl;
  CURLcode res;
  struct MemoryStruct chunk;
  chunk.memory = malloc(0);  /* will be grown as needed by the realloc above */
  memset(chunk.memory, 0, strlen(chunk.memory));
  chunk.size = 0;    /* no data at this point */
  curl = curl_easy_init();
  char error_buf[CURL_ERROR_SIZE];
  if(!curl){
    // is the case when we could not init the curl pointer itself
    fprintf(stderr, "failed to instantiate the CURL client");
    *ok  = -1;
    goto cleanup;
  }
  /* send all data to this function
  read more on the write function here :
  https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html*/
  curl_easy_setopt(curl, CURLOPT_URL, url);
  // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L); //<= this is important, but not obvious
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  if(res = curl_easy_perform(curl) != CURLE_OK){
    fprintf(stderr, "We have error response from the server :%d\n", res);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, response);
    // printf("We have response code %ld from the server\n", *response);
    goto cleanup;
  }
  else {
    /*this is easy to miss, as this is counter intuitive if there is no content
    from the server then chunk.memory would contain 'null' literal string ,
    we are just making sure that we send NULL response when such a case occurs
    If chunk.memory == "null" then contents == NULL*/
   curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, response);
   if(0!=strcmp(chunk.memory, "null")){
    //  this would send back not empty string outside
    // please use strcmp(content, "")!=0 to know if there is content
     *content = realloc(*content,strlen(chunk.memory)+1);
     memcpy(*content,chunk.memory, strlen(chunk.memory));
     goto cleanup;
   }
   else{
    //  this would send back empty string
    // please strcmp(content, "") to know if there is content or not
     memset(*content,0,strlen(*content));
   }
  }
  cleanup:
    curl_easy_cleanup(curl);
  return chunk.size;
}
unsigned short has_duplicate_keys(KeyValuePair arr[], unsigned int sz){
  // this function just returns if the there are 2 or more keys that have duplicate values
  size_t i;
  size_t j;
  for (i = 0; i < sz; i++) {
    KeyValuePair* temp  = arr;
    for(j=i+1;j<sz; j++){
      temp++;
      if(0==strcmp(arr->key,temp->key)){
        return 1;
      }
    }
    arr++;
  }
  return 0;
}
char* json_serialize(KeyValuePair payload[], unsigned int fields, int* ok){
  *ok  = 0;
  char* json_string  = malloc(0);
  if (json_string==NULL) {
    fprintf(stderr, "Out of memory\n");
    *ok  = -1;
    return "";
  }
  memset(json_string,0,strlen(json_string)); //dont miss this out
  size_t i = 0;
  if(0!=has_duplicate_keys(payload, fields)){
    fprintf(stderr, "Invalid Json Object : found duplicate fields \n");
    *ok =-1;
    return "";
  }
  for (i = 0; i < fields; i++) {
    // only if we have a valid key
    if(0!=strcmp(payload->key, "") && payload->Jsonify != NULL){
      // getting the json string of the field
      char* appendix = payload->Jsonify(payload->key, payload->strValue);
      json_string = realloc(json_string,strlen(appendix)+1+strlen(json_string));
      if (json_string == NULL) {
        fprintf(stderr, "Out of memory\n");
        *ok  = -1;
        return "";
      }
      memcpy(json_string+strlen(json_string),appendix,strlen(appendix));
    }
    payload++; //moving to the next field
  }
  // here we make that a object from the string that it is
  char* result = malloc(strlen(json_string)) ;
  if (result == NULL) {
    fprintf(stderr, "Out of memory\n");
    *ok  = -1;
    return "";
  }
  memset(result, 0, strlen(result));
  memcpy(result, json_string, strlen(json_string)-1);
  char* json_result = malloc(strlen(result)+2);
  if (json_result == NULL) {
    fprintf(stderr, "Out of memory\n");
    *ok  = -1;
    return "";
  }
  memset(json_result, 0, strlen(json_result));
  sprintf(json_result,"{%s}", result);
  // printf("%s\n",json_result);
  free(json_string);
  free(result);
  return json_result;
}
/*if you want to know how to debug
http://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)Debugging.html*/
int main_test(int argc, char const *argv[]) {
  KeyValuePair payload[] ={
    {"location","Kothrud Pune 38",jsonify_strfield},
    {"duty","Measure air pollutants",jsonify_strfield},
    {"type","RPi3B+ but not sure of the version",jsonify_strfield},
    {"random","1000",jsonify_numfield},
    {"license","true",jsonify_boolfield}
  };
  // unsigned short  result  =has_duplicate_keys(payload, 4);
  // printf("We have the result to be  : %d\n", result);
  int ok  = 0;
  char* json  = json_serialize(payload, 5, &ok);
  if (ok!=0) {
    fprintf(stderr, "failed serialization\n");
  }
  else{
    printf("%s\n",json);
  }
  // // printf("%s\n",json);
  // // printf("%s\n",result);
  // char url[]= "http://192.168.1.5:8038/api/uplink/devices/";
  // long rcode =url_post(url,json);
}
/* Quick check if device is found registered at the url
Returns         :0 if not found, 1 if found, -1 if an eror
baseUrl         :http://192.168.1.5:8038/ is the server port and ip
uuid            :this is the uuid of the device that you are looking for*/
int is_device_registered(char* baseUrl, char* uuid){
  if(strcmp(uuid, "")==0 || 0==strcmp(baseUrl, "")){return -1;}
  char url[strlen(baseUrl)+strlen(DEVICES_URL)+strlen(uuid)+2];
  sprintf(url, "%s%s%s/",baseUrl,DEVICES_URL,uuid);
  char* content = malloc(0);
  memset(content,0,strlen(content));
  long response = 0;
  int ok  =0;
  long bytes_received = url_get(url,&content,&response,&ok);
  if (ok!=0 || response!=200) {
    fprintf(stderr, "Failed request to find device registration\n");
    return -1;
  }
  else{
    if(0==strcmp(content,"")){return 0;}
    else{return 1;}
  }
  free(content);
  return -1;
}
int register_device(DeviceDetails* dd, char* baseUrl){
  if(!dd){return -1;}
  // we for now know there are 3 fields that make up device DeviceDetails
  // except the uuid - which is device identification
  KeyValuePair payload[] ={
    {"location",dd->location, jsonify_strfield},
    {"type",dd->type, jsonify_strfield},
    {"duty",dd->duty, jsonify_strfield},
  };
  // this payload now needs to seriliazed
  int ok =0;
  char* jsonPayload  = json_serialize(payload, 3, &ok);
  printf("We have the json payload ready for posting\n");
  printf("%s\n",jsonPayload);
  if(ok==0){
    // long bytes_received  =url_post(url, jsonPayload);
  }
  return 0;
}
