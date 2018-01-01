#include "license.h"

/*Searches in the license file and then tries to get the value against it
this is not an interfaced function
field         :field in the license file you are looking for
values        :value of the field in the license FILE
returns       : 0 for not found, 1 for found, and -1 for error from any operations */
int get_license_attr(char* field , char** value){
  FILE* fp;
  char buff[1024];
  int found = 0; //<< signifies if the field is found in the license file
  // since we dont want to create the file if not present , im using the flag "r"
  if ((fp = fopen(LIC_FILE_PATH, "r"))==NULL) {
    fprintf(stderr, "Failed to open license file ! - Check if the device has a valid license file\n");
    return -1; //<< error condition
  }
  while (fgets(buff,sizeof(buff), fp)!=NULL) {
    buff[strlen(buff)-1] = '\0'; //<< since fgets gets the stream with \n at the end of each line
    if (strstr(buff, field)!=NULL) {
      // we have struck the line that contains the field and we can then get the value
      char* pos = buff;
      found = 1;
      char searchStr[strlen(field)+5];
      sprintf(searchStr, "%s : ", field);
      // Im not expecting the client code to send the exact token field is expected to be uuid and not uuid :
      pos+=(strlen(searchStr));
      //<< advancing the pointer in buffer to leave out everything but the value against the attr
      // but what happens when the buffer value cannot be advanced so much
      *value  = realloc(*value, strlen(pos)+1);
      sprintf(*value, "%s", pos);
      break;
    }
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
