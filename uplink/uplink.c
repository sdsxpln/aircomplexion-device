#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
// you can reference some example from here
// post data example https://gist.github.com/jay/2a6c54cc6da442489561
// size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
//     data->append((char*) ptr, size * nmemb);
//     return size * nmemb;
// }
/*this function helps to send a GET request to the url */
struct MemoryStruct {
  char *memory;
  size_t size;
};
typedef char* (*jsonify)(char*, char*);
typedef struct structkeyvaluepair{
  char* key;
  char* strValue ;
  jsonify Jsonify;
}KeyValuePair;
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
    sprintf(new_result ,"\"%s\":\"%s\"",key, value); // printing on the variable
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
*/
long perform_post(char* url, char* payload){
  // curl -d "param1=value1&param2=value2" -X POST http://localhost:3000/data
  CURL* curl;
  CURLcode response = CURLE_OK;
  long response_code = 0;
  struct MemoryStruct chunk;
  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
  chunk.size = 0;
  char error_buf[CURL_ERROR_SIZE];
  // /* In windows, this will init the winsock stuff */
  // curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if(!curl){
    // is the case when we could not init the curl pointer itself
    perror("failed to instantiate the CURL client");
    goto cleanup;
  }
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  // the default bahaviour of CURL is to give CURLE_OK despite non 200 OK return codes
  // this mandates the curl to emit an error if thhe error from the server is returned
  curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L); //<= this is important, but not obvious
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
  response =curl_easy_perform(curl);
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
  if (response!=CURLE_OK) {
    fprintf(stderr, "%s\n",error_buf);
    goto cleanup;
  }
  printf("CURL POST success , status code %ld\n", response_code);
  // need to check if the response is in structure
  printf("We have received %d bytes in the server response \n", chunk.size);
  printf("%s\n", chunk.memory);
  cleanup:
    curl_easy_cleanup(curl);
    // curl_global_cleanup();
  return response_code;
}
long perform_get(char* url){
  CURL *curl;
  CURLcode res;
  int retCode  = 0;
  struct MemoryStruct chunk;
  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
  chunk.size = 0;    /* no data at this point */
  curl = curl_easy_init();
  long response_code =0;
  if(!curl){
    // is the case when we could not init the curl pointer itself
    perror("failed to instantiate the CURL client");
    goto cleanup;
  }
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  /* send all data to this function
  read more on the write function here :
  https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html*/
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  /* we pass our 'chunk' struct to the callback function */
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  if(res = curl_easy_perform(curl) != CURLE_OK){
    fprintf(stderr, "We have error response from the server :%d\n", res);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    printf("We have response code %ld from the server\n", response_code);
    goto cleanup;
  }
  else {
   curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
   printf("%lu bytes retrieved\n", (long)chunk.size);
   printf("The data we have received is : %s\n", chunk.memory);
   printf("We have response code %ld from the server\n", response_code);
  }
  cleanup:
    curl_easy_cleanup(curl);
  return response_code;
}

/*this iis to test the conversion of structures to jso strings and if all works fine*/
int test_jsonification(){
  KeyValuePair kvLocation={"location","Kothrud Pune 38",jsonify_strfield};
  KeyValuePair kvDuty={"duty","Measure ambient conditions",jsonify_strfield};
  KeyValuePair kvType={"type","RPi3B",jsonify_strfield};
  KeyValuePair* payload = malloc(3*sizeof(KeyValuePair));
  *(payload)= kvLocation;
  *(payload+1) = kvDuty;
  *(payload+2)= kvType;
  char* prefix_string ="[{";
  char* suffix_string = "}]";
  char* json_string  = malloc(strlen(prefix_string));
  memcpy(json_string, prefix_string,strlen(prefix_string)+1);
  /*The sizeof operator is used for arrays that are statically allocated and not
  for things that are dynamically allocated , like it is here in this case
  So it looks we woul have to keep trak of it ourselves
  https://stackoverflow.com/questions/8717267/how-to-get-the-length-of-a-dynamically-created-array-of-structs-in-c*/
  size_t i = 0,count = 3;
  for (i=0; i < count; i++) {
    char* toAppend = payload->Jsonify(payload->key, payload->strValue); //string to append
    // +1 for the \0 ending character for strings
    //check the position in memcpy we are appending the string hence
    json_string =  realloc(json_string, strlen(json_string)+strlen(toAppend)+1);
    memcpy(json_string+strlen(json_string),toAppend,strlen(toAppend)+1);
    if (i<(count-1)) {
      // we need to add ',' in between all the json values
      json_string = realloc(json_string, strlen(json_string)+1);
      memcpy(json_string+strlen(json_string), ",", strlen(",")+1);
    }
    payload++;
  }
  json_string =  realloc(json_string, strlen(json_string)+strlen(suffix_string)+1);
  memcpy(json_string+strlen(json_string),suffix_string,strlen(suffix_string)+1);
  printf("%s\n",json_string );
}
/*testing function to see if we can work with the libcurl function*/
int test_urlsend(){
  char url[]= "http://192.168.1.5:8038/api/uplink/devices/";
  perform_get(url);
  // the json string if not formatted correctly would result in internal server error 500
  // observe the escape on the \" quotation marks to make it a valid json string
  char* deviceDetails = "{\"location\":\"Pune\",\"duty\":\"measure ambient conditions\",\"type\":\"RPi3B\"}";
  // now we need to post this payload to url and see if this working
  char* invalidDeviceDetails  = "location=kothrud&duty=measurement+ambientconditions&type=RPi3B";
  perform_post(url,deviceDetails);
  // we can then try the same request with invalid payload
  perform_post(url,invalidDeviceDetails);
  return 0;
}
int main(int argc, char const *argv[]) {
  test_jsonification();
}
