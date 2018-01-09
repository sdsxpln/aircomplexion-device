#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include <sys/time.h>
#include<string.h>
#include"journal.h"

long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}
int main(int argc, char const *argv[]) {
  printf("Testing the journal working\n");
  assert(journal_flush()==0);
  // lets assume that we need to print the message in the journal file
  char* debug =  "This is a debug message from within the program";
  char* sender = "testing:main";
  journal_debug(debug,sender);
  // and then we must try appending message too
  debug = "you should see this message on the line next";
  journal_debug(debug,sender);
  // Here we can try to send out some messages as errors too
  journal_exception(debug, sender);
  exit(0);
}
