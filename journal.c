/*H**********************************************************************
* FILENAME :        journal.c             DESIGN REF:
* DESCRIPTION :
*       Implementation of device journaling - putting diagnostics messages to the journal
* PUBLIC FUNCTIONS :

* PRIVATE FUNCTIONS :

* NOTES :
*       Copyright EeensyMachines.org , All rights reserved
* AUTHOR :    kneerunjun@gmail.com        START DATE :    10 Jan 2018
*H*/
#include "journal.h"

/*flushes the journal for all the entries previously made*/
int journal_flush(){
    FILE* fp ;
    if((fp = fopen(JOURNAL_FPATH,"w"))==NULL){return -1;}
    fclose(fp);
    return 0;
}
