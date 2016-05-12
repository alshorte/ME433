#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "DEV_CONFIG.h"


int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    // do your TRIS and LAT commands here
    TRISB =  0xFFFF; // pin 4 of Port B is Button set to one for input (reset value = FFFF)
    TRISAbits.TRISA4 = 0; // pin 4 of Port A is LED set to zero for output (reset value = 079F)

    
    LATAbits.LATA4 = 0;  // initiallize LED to be on
    
    __builtin_enable_interrupts();
    
    while(1) {
	    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
        // remember the core timer runs at half the CPU speed
        
            _CP0_SET_COUNT(0);   // set core timer to 0
            while (_CP0_GET_COUNT() < 24004000){ // running at 48 MHz, Core timer at 24 MHZ
                ; // wait 0.5 ms 12002
                while(!PORTBbits.RB4) { 
                    ;
               } // Pin B4 is the button, low (FALSE) if pressed.
                
            }
            LATAINV = 0b10000 ;  // toggle LED pin 4
    }
    
    
}