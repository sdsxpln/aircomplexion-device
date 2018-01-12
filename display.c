#include "display.h"
int lcd =0;
unsigned char char_degcelcius[8] = {0x18,0x18,0x3,0x4,0x4,0x4,0x3,0x0};
unsigned char char_bulb[8] = {0x1,0x3,0xf,0xf,0xf,0x3,0x1,0x0};
unsigned char char_lightrays[8] = {0x8,0x10,0x0,0x18,0x0,0x10,0x8,0x0};
unsigned char char_heartEmpty[8] = {0x0,0x0,0xa,0x15,0x11,0xa,0x4,0x0};
unsigned char char_heartFilled[8] = {0x0,0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
void init_spl_chars(){
  if (lcd!=0) {
    lcdCharDef (lcd, 0, char_degcelcius) ;
    lcdCharDef (lcd, 1, char_bulb) ;
    lcdCharDef (lcd, 2, char_lightrays) ;
    lcdCharDef (lcd, 3, char_heartEmpty) ;
    lcdCharDef (lcd, 4, char_heartFilled) ;
  }
};
int setup_lcd_4bitmode(int rows, int cols, int rs, int e,
        int d0, int d1, int d2, int d3){
    /*remember in this 4 bit mode you need to connect the physical pins D4 to D7 but supply the values for D0 to D3*/
    lcd = lcdInit (rows,cols,4,rs,e,d0,d1,d2,d3,0,0,0,0);
    if (lcd <0) {return -1;}
    init_spl_chars();
    return 0;
}
void  setup_lcd_8bitmode(int rows, int cols, int rs, int e,
        int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7){
    lcd = lcdInit (rows,cols,8,rs,e,d0,d1,d2,d3,d4,d5,d6,d7);
    init_spl_chars();
}
void display_readings(float temp, float light, float co2, float co){
  // this takes in the specific readings instead of the string message and does the job of placing them right
  char stream[20];
  lcdClear(lcd);
  if (temp >0.0) {
    sprintf(stream, "T:%.1f\337C",temp);
    lcdPuts(lcd ,stream);
  }
  if (light>0.0) {
    sprintf(stream, " L:%.1f%%", light);
    lcdPuts(lcd ,stream);
  }
  // lcdPosition(lcd, 0,1);
  if (co2 >0.0) {
    sprintf(stream, "Co2:%.1f CO:%.1f", co2,co);
    lcdPuts(lcd ,stream);
  }
}
void lcd_clear(){
  lcdClear(lcd);
}
void lcd_message(char* msg){
  lcdClear(lcd);
  lcdPuts(lcd ,msg);
}
