#include <xc.h>           // processor SFR definitions
#include "INIT_COM.h" // initializes SPI and I2C
#include "DEV_CONFIG.h" // configure pic
#include <math.h>     // add math so you can use sine

#define NUMPTS 100       // number of point in wave

// function prototypes
void setVoltage(char channel, unsigned char voltage);
void setExpander(char pin, char level);
char getExpander(void);

int main(void) {
    unsigned char button_status;  // initialize variable for i2c read
    unsigned char reg; // initialize variable for i2c register
    unsigned char pin; // initialize variable to set output pin to

    __builtin_disable_interrupts();
        
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    __builtin_enable_interrupts();
    
    spi1_init(); // initialize SPI
    i2c2_init(); // initialize I2C
    expander_init(); // initialize pin expander inputs and outputs
    
    // make waveforms for SPI DAC
    unsigned char SWave[NUMPTS];      // initialize array of sine wave points
    unsigned char TWave[NUMPTS];      // initialize array of triangle wave points
    
    int i = 0; // initialize counter
    for(i = 0; i<NUMPTS; i++){
        
        // wave amp < 255  
        SWave[i] = (unsigned char) 127*sin(2*i*3.14/NUMPTS) + 127;
        TWave[i] = i;  
    }

    while(1) {
        // SPI DAC
        int j = 0; // initialize counter
        for (j = 0; j < NUMPTS; j++){ 
            // sign wave send to channel A
            setVoltage(0, SWave[j]);

            // triangle wave sent to channel B
            setVoltage(1,TWave[j]);
            
            // I2C IO Expander
            reg = 0x9; //GPIO register returns status of pins G0 - G7
            button_status = i2c_read(reg, SLAVE_ADDR)>>7; // check if button is pressed pin G7

            //if pressed turn on LED, if not turn off led :)
            reg = 0xA; // OLAT register sets output value
            if(button_status == 1){ // button not pressed
                pin = 0; // set G0 to low (turn off led)
                i2c_write(reg, pin, SLAVE_ADDR);
            }
            if(button_status == 0){ // button is pressed
                pin = 1; // set G0 to high (turn on led)
                i2c_write(reg, pin, SLAVE_ADDR);
            }

            _CP0_SET_COUNT(0);   // set core timer to 0
            while (_CP0_GET_COUNT() < 24004){ // running at 48 MHz, Core timer at 24 MHZ delay 1ms
                
                ;
            }
        }

        
    }
        
    return 0;
}

void setVoltage(char channel, unsigned char voltage){
    unsigned short send = 0b0000000000000000;        // initialize short to send
    send |= channel << 15;                           // set channel bit
    send |= 0b111 << 12;                             // always 111
    send |= voltage << 4;                            // set data bits to desired voltage
    send |= 0b000;                                  // can be anything lets 
    
    CS = 0;                                          // CS set to low for sending data
    spi_io(send >>8);                                // send first half of short
    spi_io(send);                                    // send second half of short
    CS = 1;
}
void setExpander(char pin, char level){
    ;
}
char getExpander(){
    ;
}