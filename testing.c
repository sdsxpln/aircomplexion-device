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
  exit(0);
}
