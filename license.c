#include "license.h"
/*H**********************************************************************
* FILENAME :        license.c             DESIGN REF:
* DESCRIPTION :
*       Implementation for device license read and write
* PUBLIC FUNCTIONS :
        int get_device_uuid(char** uuid);
        int get_device_loc(char** loc);
        int get_device_owner(char** email);
        int get_device_duty(char** duty);
        int get_device_type(char** type);
        int update_device_uuid(char* uuid);
        int update_device_loc(char* loc);
        int update_device_owner(char* email);
        int update_device_duty(char* duty);
        int update_device_type(char* type);
        int authorize();
* PRIVATE FUNCTIONS :
        int word_match(char* line, char* word, size_t* pos)
        int get_license_attr(char* field , char** value)
        int write_license_attr(char* attr, char* val)
* NOTES :
*       Copyright EeensyMachines.org , All rights reserved
* AUTHOR :    kneerunjun@gmail.com        START DATE :    04 Jan 2018
* CHANGES :
      + device license can be read
      + device license can now be written to provided the license file is in specific recommended format
      + word match capability would let find the attributes in the license faster
*H*/

/*given a line and a phrase to match, this can say if the exact word is matched for the first instance
This function is yet not improvised for multiple matches
line          :line in which the word woudl be parsed
word          :word phrase that we are looking for needs to be atleast 2 bytes
pos           : position at which the word is encountered*/
int word_match(char* line, char* word, size_t* pos){
  int wordMatch  =0;
  char* sender = "license:word_match";
  *pos  = -1; //<< assuming that we have no matches yet
  if(strlen(word)<2){
    // fprintf(stderr, "Its not a word that you are looking for, is it ?\n");
    journal_exception("Word being searched is perhaps a chracter", sender);
    return -1;
  } //<< if its not a word at all
  char* result = strstr(line,word);
  if (result != NULL) {
    // move ahead by the word length to get to suspected word spotting
    *pos = strlen(line)-strlen(result); // << position at which this was spotted
    if(strcmp(line, result)==0 || *(line+(*pos)-1)==' '){
      //<< this actually gives us the indication that we have found a valid word from left side
      // << we can then do to see if its a word on the right side too
      size_t wordEnd  = *pos+strlen(word);
      if(*(line+wordEnd)==' ' || *(line+wordEnd)=='\0'){
        journal_debug("Complete word found", sender);
        return 1;
      }
      else{
        // <<this is the case when the word is part of some other word from the right
        journal_debug("Phrase found, but not a word", sender);
        return 0;
      }
    }
    else{
      journal_debug("Phrase found, but not a word", sender);
      // <<this is the case when the word is part of some other word from the left
      return 0;
    }
  }
  else{
    // << phrase is not found at all
    // printf("Word not found in the string \n");
    journal_debug("Phrase sought is not found",sender);
    return 0;
  }
}
/*Searches in the license file and then tries to get the value against it
this is not an interfaced function
field         :field in the license file you are looking for
values        :value of the field in the license FILE
returns       : 0 for not found, 1 for found, and -1 for error from any operations */
int get_license_attr(char* field , char** value){
  char* sender  = "license:get_license_attr";
  FILE* fp;
  char buff[MAX_BUFF];
  int found = 0; //<< signifies if the field is found in the license file
  char* fldValSep  = " : "; //<< this refers to the shape o the license file
  // since we dont want to create the file if not present , im using the flag "r"
  if ((fp = fopen(LIC_FILE_PATH, "r"))==NULL) {
    // fprintf(stderr, "Failed to open license file ! - Check if the device has a valid license file\n");
    journal_exception("Failed to open license file", sender);
    return -1; //<< error condition
  }
  while (fgets(buff,sizeof(buff), fp)!=NULL) {
    buff[strlen(buff)-1] = '\0'; //<< since fgets gets the stream with \n at the end of each line
    // match the word here and send the value at  pos +3 from the attr
    size_t pos = -1; //<<position on the buffer line where the attribute is found
    if(word_match(buff,field, &pos)<=0){
      found  =0;
      memset(buff,0,MAX_BUFF);
      continue;
    }
    if(pos<0){
      // fprintf(stderr, "failed to get attribute position in the license file\n");
      journal_exception("Failed to read license attribute, check license file formatting", sender);
      return -1;
    }
    // << this is where we are sure we have struck the attribute exactly
    found  =1;
    size_t valBegin =0, valEnd = 0;
    valBegin = pos + strlen(field) + strlen(fldValSep);
    valEnd = strlen(buff);
    *value = calloc(valEnd-valBegin+1, sizeof(char));
    strncpy(*value,buff+valBegin ,valEnd-valBegin);
    journal_debug("Got value of the license attribute", sender);
    break;
  }
  fclose(fp);
  return found == 0 ? 0:1;
}
/*this helps to write a specific section in the license file.
attr          : attribute that need to update
val           : new updated value of the attribute
return        : 0 for normal, >0 for invalidated inputs, <0 for exceptions*/
int write_license_attr(char* attr, char* val){

  FILE* fp;
  char buff [MAX_BUFF]; //<< hoping the license file does not outrun this
  char newBuff[MAX_BUFF];
  int attrSpotted =0;
  size_t pos = -1; //<<position on the buffer line where the attribute is found

  if(strcmp(attr,"")==0 || strcmp(val,"")==0){
    fprintf(stderr, "Attr or the value of the attr cannot be empty\n");
    return 1;
  }
  // never would we want to append the file if does not exists
  /*license files are to created by hand and this comes under the licensing purview*/
  if ((fp=fopen(LIC_FILE_PATH, "r"))==NULL) {
    /*this is when the file could not be opened - */
    fprintf(stderr, "Failed to open license file\n");
    return -1;
  }
  // lets read it line by line till we find the line that contains the attribute
  while (fgets(buff,sizeof(buff),fp)!=NULL) {
    buff[strlen(buff)-1] = '\0'; //<< since fgets gets the stream with \n at the end of each line
    if(word_match(buff,attr, &pos)>0){
      if(pos>=0){
        attrSpotted =1;
        char replacement[100]; //<< hoping that the line size does not outrun
        sprintf(replacement, "%s : %s\n", attr, val);
        // and this replacement then goes into the newbuff
        strcat(newBuff,replacement);
        continue;
      }
    }
    /*if the line is not containing the attribute we are looking for  then just copy into new buffer*/
    strcat(buff, "\n"); //<< remember this new line chracter wa replaced when read from the file
    strcat(newBuff,buff);
  }
  fclose(fp); //<< closing the file when done
  // newbuffer is now formed
  /*file being opened with w flag which means new file with creation opened for writing ..
  previous contents of the file are washed away*/
  if ((fp=fopen(LIC_FILE_PATH, "w"))==NULL) {
    /*this is when the file could not be opened - */
    fprintf(stderr, "Failed to write to license file\n");
    return -1;
  }
  fputs(newBuff, fp);
  fclose(fp); //<< closing the file when done
  return 0;
}
int device_json(char** payload){
  char* uuid = "";
  char* location ="";
  char* owner="";
  char*  epoch="";
  char*  duty="";
  char*  type="";
  *payload  = calloc(1024, sizeof(char)); //<< something large enough
  if(get_license_attr("uuid",&uuid)<0){fprintf(stderr, "Failed to get uuid of the device\n");return -1;}
  sprintf(*payload,"\"uuid\" : \"%s\",",uuid);
  if(get_license_attr("location",&location)<0){fprintf(stderr, "Failed to get location of the device\n");return -1;}
  sprintf((*payload)+strlen(*payload), "\"location\" : \"%s\",", location);
  if(get_license_attr("duty",&duty)<0){fprintf(stderr, "Failed to get duty of the device\n");return -1;}
  sprintf((*payload)+strlen(*payload), "\"duty\" : \"%s\",", duty);
  if(get_license_attr("owner",&owner)<0){fprintf(stderr, "Failed to get owner of the device\n");return -1;}
  sprintf((*payload)+strlen(*payload), "\"owner\" : \"%s\",", owner);
  if(get_license_attr("type",&type)<0){fprintf(stderr, "Failed to get owner of the device\n");return -1;}
  sprintf((*payload)+strlen(*payload), "\"type\" : \"%s\",", type);
  if(get_license_attr("epoch",&epoch)<0){fprintf(stderr, "Failed to get epoch of the device\n");return -1;}
  sprintf((*payload)+strlen(*payload), "\"epoch\" : \"%s\"", epoch);
  char buff[strlen(*payload)+3];
  sprintf(buff, "{%s}",*payload);
  *payload = calloc(strlen(buff), sizeof(char));
  strcpy(*payload, buff);
  return 0;
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
int get_license_server(char** baseUrl){
  return get_license_attr("server", baseUrl);
}

int update_device_uuid(char* uuid){
  return write_license_attr("uuid",uuid);
}
int update_device_loc(char* loc){
  return write_license_attr("location",loc);
}
int update_device_owner(char* email){
  return write_license_attr("owner",email);
}
int update_device_duty(char* duty){
  return write_license_attr("duty",duty);
}
int update_device_type(char* type){
  return write_license_attr("type",type);
}
