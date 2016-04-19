#include<xc.h>           // processor SFR definitions
#include "INIT_COM.h" // initializes SPI and I2C

#define NUMPTS 200;       // number of point in wave

void setVoltage(char channel, char voltage);


int main(void) {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    spi_init(); // initialize SPI
    
    // make waveforms
    SWave[NUMPTS];      // initialize array of sine wave points
    TWave[NUMPTS];      // initialize array of triangle wave points
    

    while(1) {
        for (int i = 0, i < NUMPTS, i++){ 
            // sign wave send to channel A
            setVoltage(0,SWave[i]);

            // triangle wave sent to channel B
            setVoltage(1,TWave[i]);
        }
    }
        
    return 0;
}

void setVoltage(char channel, char voltage){
    unsigned short send = 0b0000000000000000;        // initialize short to send
    send |= channel << 15;                           // set channel bit
    send |= 0b111 << 12;                             // always 111
    send |= voltage << 4;                            // set data bits to desired voltage
    send |= 0b000;                                  // can be anything lets 
    
    CS = 0;                                          // check if SPI is working
    spi_io(send >>8);                                // send first half of short dd
    spi_io(send);                                    // send second half of short
    CS = 1;
}
