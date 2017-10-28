#include "alerts.h"
#include <unistd.h>
#include <wiringPi.h>
/*this is only to get the GPIO pins ready , up and running*/
int bLED=0, rLED=0, buzz=0;
void setup_alert(int bluegpio, int redgpio, int buzzgpio){
  wiringPiSetupGpio();
  pinMode (redgpio, OUTPUT) ; digitalWrite(redgpio, LOW);
  pinMode (bluegpio, OUTPUT) ; digitalWrite(bluegpio, LOW);
  pinMode (buzzgpio, OUTPUT) ; digitalWrite(buzzgpio, LOW);
  bLED = bluegpio;rLED=redgpio;buzz=buzzgpio;
  return;
}
void alert(int* ok, int level, int muted){
  static int  currLevel = -1;
  *ok=0;
  if (bLED==0 || rLED ==0 || buzz==0) {
    /* typical case when we have not yet setup the gpio pins*/
    *ok =-1; return;
  }
  if(level!=currLevel){
    // only if there is change in the current level
    switch (level) {
    case 0:
      digitalWrite(bLED, HIGH);
      digitalWrite(rLED, LOW);
      digitalWrite(buzz, LOW);
      currLevel = 0;break;
    case 1:
      digitalWrite(bLED, HIGH);
      digitalWrite(rLED, HIGH);
      digitalWrite(buzz, LOW);
      currLevel = 1;break;
    case 2:
      digitalWrite(bLED, LOW);
      digitalWrite(rLED, HIGH);
      currLevel = 2;break;
    }
  }
  if(muted<=0 && currLevel>1){
    digitalWrite(buzz, HIGH);
    sleep(1);
    digitalWrite(buzz, LOW);
  }
}
