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
    sprintf(new_result ,"\"%s\": \"%s\",",key, value);
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
    sprintf(new_result ,"\"%s\": %s,",key, value);
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
  sprintf(new_result ,"\"%s\": %s,",key, value);
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
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
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
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
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
int to_json(KeyValuePair payload[], int fields ,char** json){
  size_t i =0;
  char* lDelim = "{";
  char* result = (char*)malloc(2*sizeof(char)); //<< 2*sizeof(char), dont use strlen(lDelim)+1 - the character may not be one byte always
  if (result == NULL) {
    fprintf(stderr, "%s\n", "Out of memory , Cannot convert to json");
    return -1;
  }
  memcpy(result, lDelim, strlen(result));
  for (i = 0; i < fields; i++) {
    if (strcmp(payload->key, "")!=0 && strcmp(payload->strValue, "")!=0) {
      char* fieldsAsJson  = payload->Jsonify(payload->key, payload->strValue);
      result = (char*)realloc(result, strlen(result)+strlen(fieldsAsJson)+1);
      memcpy(result+strlen(result),fieldsAsJson,strlen(fieldsAsJson));
    }
    payload++;
  }
  // now to remove the leading ',' and add the rDelim character
  if (strlen(result)>1) {
    /*we want to carry this operation when in general cases where we have the happy path
    the case where the json object is not empty*/
    char* temp = result;
    while(*temp!='\0'){temp++;}
    *(temp-1)='}';//<< replacing the last ',' with delimiting '}'
  }
  else{
    /*case when we have the json empty object*/
    result = realloc(result, 2*sizeof(char)+1) ;
    memcpy(result,"{}",strlen("{}"));
  }
  *json = realloc(*json, strlen(result)+1);
  *json  = strdup(result);
  // Never free result, despite beig allocated here
  // strdup() is shallow duplication and the same memory is being referenced outside
  // some delta change
  return 0;
}
/*if you want to know how to debug
http://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)Debugging.html*/
/*Finds and splits a string into 2 sections on the first occurence of the token
This will not find multiple token occurences and as such would need a better function
But for now this forms the basis of de-serialize the strings we receive from http response
Please note, the split result will NOT include the token
toSplit       :this is the string we intend to search and explode
token           :tsubstring we are looking to demarkate and break the string
left            :left side of the split string
right           :right side of the split string
rightClip       :right side of the split string can be clipped ,
if entire string is desired issue 0, negative integers cannot go in here
Returns 0 for no errors and negative for errors
*/
int splitstr_token(char* toSplit,char* token,char** left,char** right,size_t rightClip){
  size_t tokenSz  = strlen(token); //we are going to proceed as this much in each
  if(tokenSz==0 || strlen(toSplit)==0){return -1;} //exiting incase of bad inputs
  char* target = toSplit;
  // so if the string is not found the split portions are all empty
  memset(*left, 0, strlen(*left));
  memset(*right, 0, strlen(*right));
  while (*target != '\0' && (strlen(target)>=tokenSz)) {
    /*Target would iassessed section wise - section size determined bu the token*/
    char* strSection  = malloc(tokenSz+1);
    memset(strSection,0,tokenSz+1);
    memcpy(strSection,target, tokenSz);//section formed
    printf("Substring \t\t%s\n", strSection);
    if(0==strcmp(strSection,token)){
      // token is the section .. token found in the string
      // form the left side of the string, target is still on the left of the token
      size_t leftLen = strlen(toSplit)-strlen(target);
      *left = realloc(*left,leftLen+1);
      memset(*left,0,leftLen+1);
      memcpy(*left,toSplit,leftLen);
      target += tokenSz;//target is now at the end of the token
      if(rightClip<=0){rightClip = strlen(target);}
      *right = realloc(*right,rightClip+1);
      memset(*right,0,rightClip+1);
      memcpy(*right,target,rightClip);
      // now forming the clipped result
      break;
    }
    // Moving to next section
    //move on the string being searched from the next character onwards to section length
    // free(strSection);
    target++;
  }
  if (0==strcmp(*left,"") && 0==strcmp(*right,"")) {
    return 0;
  }
  else{return 1;}
}
/* Quick check if device is found registered at the url
Returns         :0 if not found, 1 if found, -1 if an eror
baseUrl         :http://192.168.1.5:8038/ is the server port and ip
uuid            :this is the uuid of the device that you are looking for*/
int is_device_registered(char* baseUrl, char* uuid){
  if(strcmp(uuid, "")==0 || 0==strcmp(baseUrl, "")){return -1;}
  char url[strlen(baseUrl)+strlen(DEVICES_URL)+strlen(uuid)+2];
  sprintf(url, "%s%s%s/",baseUrl,DEVICES_URL,uuid);
  char* content = (char*)malloc(0);
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
  return -1;
}
/*This enables the registry of the new device
payload       :the device details as an array of KeyValuePair
baseUrl       :baseUrl that needs to be hit
uuid          :the uuid of the newly registered device
returns 0 on success and negative on failure
*/
int register_device(KeyValuePair payload[], char* baseUrl, char** uuid){
  char url[strlen(baseUrl)+strlen(DEVICES_URL)+1]; //complete url
  sprintf(url, "%s%s",baseUrl,DEVICES_URL); //making the complete  url
  char* content = malloc(sizeof(char)); // this the body of the response
  // memset(content,0, strlen(content)); //setting the body of response to ""
  long response_code = 0; // HTTP response code
  int ok =0; //this function overall status
  char * left = (char*)malloc(sizeof(char)); //response body fragment left
  char* right = (char*)malloc(sizeof(char)); //response body fragment right
  char* jsonPayload  = (char*)malloc(sizeof(char));
  *uuid = (char*)malloc(sizeof(char)*1);  // preparing the output
  if (to_json(payload, 3, &jsonPayload)!=0){
    fprintf(stderr, "Failed to formulate the json object \n");
    return -1;
  };
  long bytesRecv =url_post(url, jsonPayload, &content ,&response_code, &ok);
  if (ok ==0 && response_code ==200 && 0!=strcmp(content, "")) {
    printf("Response code is %d\n",response_code );
    if (splitstr_token(content ,"\"uuid\": \"", &left, &right, 36) !=0) {
      if(0!=strcmp(right,"")){
        // printf("%s\n", right);
        *uuid  = realloc(*uuid,strlen(right)+1);
        memcpy(*uuid, right, strlen(right));
        return 0;
      }
      else{
        fprintf(stderr, "Could not get the uuid of the newly posted device, Device may have been posted\n");
        return -1;
      }
    }
    else{
      fprintf(stderr, "Device might have been posted, error parsing response from the server\n");
      return -1;
    }
  }
  else{
    fprintf(stderr, "Failed server request to post the new device details \n");
    return -1;
  }
  return 0;
}
