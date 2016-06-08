#include "init.h"

// blink LED

void LED_init(){
TRISAbits.TRISA4 = 0;
}

void HBridge_init(){
    //set up phase bits
    ANSELBbits.ANSB13 = 0;      // Turn off analog on B13 so it can be used for output
    TRISBbits.TRISB13 = 0;      // set B13 to output
    TRISBbits.TRISB9 = 0;       // set B9 to output
    LATBbits.LATB13 = 1;        // set to forward direction
    LATBbits.LATB13 = 1;        // set to forward direction
    
     //Set up pins for OC1 and OC2, to output PWM at 1kHz using timer2, with a PR2 value between 1000 and 10000.
    // OC1 can be pins A0 B3 B4 B15 B7 
    // OC2 can be pins A1 B5 B1 B11 B8 A8 A9
    // set pin XX to be OC1 and pin XX to be OC2
    ANSELBbits.ANSB15 = 0;      // Turn off analog on B15 so it can be used for OC1
    //ANSELBbits.ANSB8 = 0;       // turn off analog on B8 so it can be used for OC2
    RPB15Rbits.RPB15R = 0b0101;   //set OC1 to be pin B15
    RPB8Rbits.RPB8R = 0b0101;   //set OC2 to be pin B8
    
    // set up Timer and PWM
    T2CONbits.TCKPS = 4;        // Timer2 prescaler N=16 (1:16)
    PR2 = 2999;                 // period = (PR2+1) * N * 20.833 ns = 1ms, 1kHz
    TMR2 = 0;                   // initial TMR2 count is 0
    
    // set up OC1
    OC1CONbits.OCTSEL = 0;      // set timer 2 to output compare
    OC1CONbits.OCM = 0b110;     // PWM mode without fault pin; other OC1CON bits are defaults
    OC1RS = 1500;               // duty cycle = OC1RS/(PR2+1) = 50%
    OC1R = 1500;                // initialize before turning OC1 on; afterward it is read-only
    
    //set up OC2
    OC2CONbits.OCTSEL = 0;      // set timer 2 to output compare
    OC2CONbits.OCM = 0b110;     // PWM mode without fault pin; other OC1CON bits are defaults
    OC2RS = 1500;               // duty cycle = OC2RS/(PR2+1) = 50%
    OC2R = 1500;                // initialize before turning OC1 on; afterward it is read-only
    
    T2CONbits.ON = 1;           // turn on Timer2
    OC1CONbits.ON = 1;          // turn on OC1
    OC2CONbits.ON = 1;          // turn on OC2    
    
}