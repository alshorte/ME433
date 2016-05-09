#include <xc.h>             // processor SFR definitions
#include "DEV_CONFIG.h"     //configure pic
#include "ILI9163C.h"       //LCD functions

// function prototypes
void draw_character(unsigned char c);
void draw_string();

int main(void){
    
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
    LCD_clearScreen(0);   // clear screen to white
    // initialize x, y position to write to?
    
    // receive from user what to write to screen
    
    
    // write to screen
    draw_string(msg);
      

}

void draw_character(unsigned char c){
    unsigned short colour;
    unsigned char = draw;
    
    // look up ASII character
    draw = ASCII[desired character][];
    
    
    for (j = 0; j < 4; j++) {       // which column
        for(i = 0; i < 7; i++){     // which bit in that column
            
            if(draw[i][j] == 0){
                colour = 0;         // set colour to white
            }
            else{
                colour = 1;         // set colour to black
                }
            LCD_drawPixel(x,y,colour);
           
        }
    }   
}

void draw_string(msg){
    while(msg[q]!= 0){  // continue until reaching null character
        draw_character(msg[q]);
        // also send x, y position?
        // increment x and y positions
        x = x + 5;
        y = y + 7;
        q = q+1;
    }
}
