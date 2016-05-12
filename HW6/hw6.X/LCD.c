#include "ILI9163C.h"       //LCD functions

void draw_character(unsigned char c, unsigned char x, unsigned char y){
    unsigned short colour;
    unsigned char draw;
    char i = 0;
    char j = 0;
    
    for (j = 0; j < 5; j++) {       // which column
        draw = ASCII[c-0x20][j]; // specify the byte
        for(i = 0; i < 8; i++){     // which bit in that column   
            if((draw >> i)&1 == 1){ // shift draw to look at only one pixel 
                colour = RED;         // set colour to RED
            }
            else {
                colour = WHITE;         // set colour to WHITE
                }
            // check if pixel exists before writing???
            LCD_drawPixel(x+j,y+i,colour);  
        }
    }   
}

void draw_string(unsigned char *m, unsigned char x, unsigned char y){
    char q = 0;         // initialize character counter
    while(m[q]!= 0){  // continue until reaching null character
        draw_character(m[q], x, y);
        x = x + 5; // move one column over from last character
        q = q+1;
    }
}

