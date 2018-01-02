#include "license.h"


/*given a line and a phrase to match, this can say if the exact word is matched for the first instance
This function is yet not improvised for multiple matches
line          :line in which the word woudl be parsed
word          :word phrase that we are looking for needs to be atleast 2 bytes
pos           : position at which the word is encountered*/
int word_match(char* line, char* word, size_t* pos){
  int wordMatch  =0;
  *pos  = -1; //<< assuming that we have no matches yet
  if(strlen(word)<2){fprintf(stderr, "Its not a word that you are looking for, is it ?\n");return -1;} //<< if its not a word at all
  char* result = strstr(line,word);
  if (result != NULL) {
    // move ahead by the word length to get to suspected word spotting
    *pos = strlen(line)-strlen(result); // << position at which this was spotted
    if(strcmp(line, result)==0 || *(line+(*pos)-1)==' '){
      //<< this actually gives us the indication that we have found a valid word from left side
      // << we can then do to see if its a word on the right side too
      size_t wordEnd  = *pos+strlen(word);
      if(*(line+wordEnd)==' ' || *(line+wordEnd)=='\0'){
        //<< perfect word
        // printf("Word match success !!\n");
        return 1;
      }
      else{
        // <<this is the case when the word is part of some other word from the right
        // printf("Word match fail :Right !!\n");
        return 0;
      }
    }
    else{
      // <<this is the case when the word is part of some other word from the left
      // printf("Word match fail :Left !!\n");
      return 0;
    }
  }
  else{
    // << phrase is not found at all
    // printf("Word not found in the string \n");
    return 0;
  }
}
/*Searches in the license file and then tries to get the value against it
this is not an interfaced function
field         :field in the license file you are looking for
values        :value of the field in the license FILE
returns       : 0 for not found, 1 for found, and -1 for error from any operations */
int get_license_attr(char* field , char** value){
  FILE* fp;
  char buff[1024];
  int found = 0; //<< signifies if the field is found in the license file
  char* fldValSep  = " : "; //<< this refers to the shape o the license file
  // since we dont want to create the file if not present , im using the flag "r"
  if ((fp = fopen(LIC_FILE_PATH, "r"))==NULL) {
    fprintf(stderr, "Failed to open license file ! - Check if the device has a valid license file\n");
    return -1; //<< error condition
  }
  while (fgets(buff,sizeof(buff), fp)!=NULL) {
    buff[strlen(buff)-1] = '\0'; //<< since fgets gets the stream with \n at the end of each line
    // match the word here and send the value at  pos +3 from the attr
    size_t pos = -1; //<<position on the buffer line where the attribute is found
    if(word_match(buff,field, &pos)<=0){
      found  =0;
      continue;
    }
    if(pos<0){
      fprintf(stderr, "failed to get attribute position in the license file\n");
      return -1;
    }
    // << this is where we are sure we have struck the attribute exactly
    found  =1;
    size_t valBegin =0, valEnd = 0;
    valBegin = pos + strlen(field) + strlen(fldValSep);
    valEnd = strlen(buff);
    *value = calloc(valEnd-valBegin+1, sizeof(char));
    strncpy(*value,buff+valBegin ,valEnd-valBegin);
    break;
  }
  fclose(fp);
  return found == 0 ? 0:1;
}
int get_device_type(char** type){
  char* field  = "type";
  int result   = 0;
  if((result = get_license_attr(field, type))<=0){
    fprintf(stderr, "Failed to get the duty of the device \n");
    memset(*type,0,sizeof(char));
    return -1;
  }
  return 0;
}
int get_device_duty(char** duty){
  char* field  = "duty";
  int result   = 0;
  if((result = get_license_attr(field, duty))<=0){
    fprintf(stderr, "Failed to get the duty of the device \n");
    memset(*duty,0,sizeof(char));
    return -1;
  }
  return 0;
}
int get_device_owner(char** email){
  char* field  = "owner";
  int result   = 0;
  if((result = get_license_attr(field, email))<=0){
    fprintf(stderr, "Failed to get the uuid of the device \n");
    memset(*email,0,sizeof(char));
    return -1;
  }
  return 0;
}
int get_device_loc(char** loc){
  char* field  = "location";
  int result   = 0;
  if((result = get_license_attr(field, loc))<=0){
    fprintf(stderr, "Failed to get the uuid of the device \n");
    memset(*loc,0,sizeof(char));
    return -1;
  }
  return 0;
}
int get_device_uuid(char** uuid){
  char* field  = "uuid";
  int result   = 0;
  if((result = get_license_attr(field, uuid))<=0){
    fprintf(stderr, "Failed to get the uuid of the device \n");
    memset(*uuid,0,sizeof(char));
    return -1;
  }
  return 0;
}
