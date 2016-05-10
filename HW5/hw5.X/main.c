#include <xc.h>             // processor SFR definitions
#include "DEV_CONFIG.h"     //configure pic
#include "ILI9163C.h"       //LCD functions

// function prototypes
void draw_character(unsigned char c, unsigned char x, unsigned char y);
void draw_string(unsigned char *m, unsigned char x, unsigned char y);

int main(void){
    char msg[100];      // initialize string array
    
    __builtin_disable_interrupts();

      __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

      // 0 data RAM access wait states
      BMXCONbits.BMXWSDRM = 0x0;

      // enable multi vector interrupts
      INTCONbits.MVEC = 0x1;

      // disable JTAG to get pins back
      DDPCONbits.JTAGEN = 0;

    __builtin_enable_interrupts();
      
    SPI1_init();              // initialize SPI and LCD
    LCD_init();
    LCD_clearScreen(WHITE);   // clear screen to white
    
    
    // initialize x, y position to write to?
    char x = 28;
    char y = 32;
    
    // hard code something character array sprintf
    int n = 1337;       // numerical variable to print
    sprintf(msg,"Hello world %d! /n",n);
    
    // write to screen
    draw_string(msg, x, y);
}

void draw_character(unsigned char c, unsigned char x, unsigned char y){
    unsigned short colour;
    unsigned char draw;
    char i = 0;
    char j = 0;
    
    for (j = 0; j < 5; j++) {       // which column
        draw = ASCII[c-0x20][j]; // specify the byte
        for(i = 0; i < 8; i++){     // which bit in that column
            
            if(draw >> (7-i)&1 == 0){ // shift draw to look at only one pixel
                colour = WHITE;         // set colour to white
            }
            else{
                colour = BLACK;         // set colour to black
                }
            // check if pixel exists before writing
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
