#include "upstart.h"
pid_t pid;
int loopRestart=0;
void before_kill(int sig);
void on_force_restart();
void startloop_wait();
void on_force_shutd();

int main(int argc, char const *argv[]) {
  // setting up the listeners for the signals
  signal(SIGINT, before_kill);
  signal(SIGTERM, before_kill);
  signal(SIGHUP, before_kill);
  // signal(SIGKILL, before_kill); //<< SIGKILL and SIGSTOP cannot be caught blocked or ignored
  wiringPiSetupGpio();
  pinMode(RESTART_GPIO, INPUT);
  pinMode(SHUTD_GPIO, INPUT);
  pullUpDnControl(RESTART_GPIO, PUD_UP);
  pullUpDnControl(SHUTD_GPIO, PUD_UP);
  wiringPiISR(RESTART_GPIO,INT_EDGE_FALLING, &on_force_restart);
  wiringPiISR(SHUTD_GPIO,INT_EDGE_FALLING, &on_force_shutd);
  startloop_wait();
  while (loopRestart ==1){
    startloop_wait();
  }
  return 0;
}
void before_kill(int sig){
  loopRestart =0;
  if (pid>0) {
    kill(pid, SIGTERM);
  }
}
void on_force_restart(){
  /*this is when you want to restart the sensing loop */
  static unsigned long lastHit = 0;
  unsigned long latestHit  = millis();
  if(latestHit -lastHit > DEBOUNCE){
    printf("Restarting sensing loop..\n");
    // we are making arrangements for the old process to be recycled.
    loopRestart =1;
    kill(pid, SIGTERM); //back to to waitpid call
  }
  lastHit = latestHit;
}
void startloop_wait(){
  pid=fork();
  if (pid==0) {
    // static char *argv[]={"sh", "-c", "sudo", "/home/pi/src/aircomplexion-device/bin/sensing"};
    // /*execv would replace the child process with the running sensing loop*/
    // if ((execvp(argv[0],argv))==-1){
    //   perror("Failed to start the looping process");
    //   exit(127);
    // }
    /*Whats important to note that is that the program has to be run with sudo profile.
    since the sensing loop employs PWM cycle and if not run with sudo profile it just hangs up*/
    if(system("sudo /home/pi/src/aircomplexion-device/bin/sensing")<0){
      fprintf(stderr, "Failed to start the child sensing process\n");
      exit(127);
    }
  }
  else{
    int status;
    waitpid(pid, &status, 0);
  }
}
void on_force_shutd(){
  loopRestart =0;
  kill(pid, SIGTERM);
  system("sudo shutdown -h now");
}
