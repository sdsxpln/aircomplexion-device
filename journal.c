/*H**********************************************************************
* FILENAME :        journal.c             DESIGN REF:
* DESCRIPTION :
*       Implementation of device journaling - putting diagnostics messages to the journal
* PUBLIC FUNCTIONS :
        int journal_debug(char* message, char* sender);
        int journal_exception(char* errMessage, char* sender);
        int journal_flush();
* PRIVATE FUNCTIONS :

* NOTES :
*       Copyright EeensyMachines.org , All rights reserved
* AUTHOR :    kneerunjun@gmail.com        START DATE :    10 Jan 2018
*H*/

#include "journal.h"
/*To spit out the exceptions from various parts of the code
If defined the JRNL preprocessor directives , this would emit all the information to the journal file */
int journal_exception(char* errMessage,char* sender){
  #ifdef JRNL
    FILE* fp ;
    // opening the file for appending data
    if ((fp=fopen(JOURNAL_FPATH,"ab+"))==NULL) {
      return -1;
    }
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    time_t t_sc; time(&t_sc);
    fprintf(fp, "!E %s\t[%d-%d-%d %d:%d:%d]\t%s\n",sender,tm.tm_year + 1900,tm.tm_mon + 1, \
    tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec,errMessage);
    fclose(fp);
  #endif
  return 0;
}
int journal_debug(char* message, char* sender){
  #if(JRNL && JRNL>1)
    FILE* fp ;
    // opening the file for appending data
    if ((fp=fopen(JOURNAL_FPATH,"ab+"))==NULL) {
      return -1;
    }
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    time_t t_sc; time(&t_sc);
    fprintf(fp, ">> %s\t[%d-%d-%d %d:%d:%d]\t%s\n",sender,tm.tm_year + 1900,tm.tm_mon + 1, \
    tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec,message);
    fclose(fp);
  #endif
  return 0;
}
/*flushes the journal for all the entries previously made*/
int journal_flush(){
    FILE* fp ;
    if((fp = fopen(JOURNAL_FPATH,"w"))==NULL){return -1;}
    fclose(fp);
    return 0;
}
