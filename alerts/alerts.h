/*this is to access the  visual and sonic alerts for the co2 sensing system*/
#ifndef LM35_H
#define LM35_H
/*defines the level of alertness. The idea is to indicate in a xo*/
typedef enum {LOW=0, MEDIUM=1, HIGH=2}ALERTLVL; //this defines how severe is the alert
void setup_alert(int bluegpio, int redgpio, int buzzgpio); //this would just setup the alerting gpio.
void alert(int* ok, int level, int muted);
#endif
