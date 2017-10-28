/*this is to access the  visual and sonic alerts for the co2 sensing system*/
#ifndef DISPLAY_H
#define DISPLAY_H
/*this is to setup the LCD from the client  so as to get the respetive GPIO ready
@rows       : rowsd available on the LCD
@cols       : these are the number of columns available on the LCD
@bits       :Bits is the number of bits wide on the interface (4 or 8).
In 8 bit mode, each command or character is sent to the LCD as a single byte (8 bits) of data.The byte travels in parallel
over 8 data wires,with each bit travelling through it’s own wire.8 bit mode has twice the bandwidth as 4 bit mode,
which in theory translates to higher data transfer speed.The main downside to 8 bit mode is that it uses up a lot of GPIO pins
refer here http://www.circuitbasics.com/raspberry-pi-lcd-set-up-and-programming-in-c-with-wiringpi/
We would prefer this to happen in the 4 bit mode
@rs         :is the RS pin on the displayed
@e          :Is the strobe(E) pin on lcd displaty
*/
void  setup_lcd_4bitmode(int rows, int cols, int rs, int e,
        int d0, int d1, int d2, int d3);
void  setup_lcd_8bitmode(int rows, int cols, int rs, int e,
                int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7);
void display_readings(float temp, float light, float co2);
void lcd_clear();
void lcd_message(char* msg);
#endif
