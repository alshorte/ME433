#include <xc.h>           // processor SFR definitions
#include "INIT_COM.h" // initializes SPI and I2C
#include <math.h>     // add math so you can use sine

#define NUMPTS 100       // number of point in wave
#define SLAVE_ADDR 0x32 // for i2c

void setVoltage(char channel, char voltage);
void setExpander(char pin, char level);
char getExpander();

int main(void) {
    // initialize variables for i2c
    char buf[100] = {};                       // buffer for sending messages to the user
    unsigned char master_write0 = 0xCD;       // first byte that master writes
    unsigned char master_write1 = 0x91;       // second byte that master writes
    unsigned char master_read0  = 0x00;       // first received byte
    unsigned char master_read1  = 0x00;       // second received byte

    __builtin_disable_interrupts();
        //i2c_slave_setup(SLAVE_ADDR);              // init I2C5, which we use as a slave 
                                                   //  (comment out if slave is on another pic)
        i2c_master_setup();                       // init I2C2, which we use as a master
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    spi1_init(); // initialize SPI
    i2c2_init(); // initialize I2C
    expander_init(); // initialize pin expander inputs and outputs
    
    // make waveforms for SPI DAC
    unsigned char SWave[NUMPTS];      // initialize array of sine wave points
    unsigned char TWave[NUMPTS];      // initialize array of triangle wave points
    
    
    int i = 0; // initialize counter
    for(i = 0; i<NUMPTS; i++){
        
        // wave amp < 255  
        SWave[i] = (unsigned char) 127*sin(i*3.14/NUMPTS) + 127;
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
            
            // add 1 ms delay
            _CP0_SET_COUNT(0);   // set core timer to 0
            while (_CP0_GET_COUNT() < 24004){ // running at 48 MHz, Core timer at 24 MHZ
                ; // wait 1 ms
            }
        }
        
        // I2C IO Expander Function
        i2c_master_start();                     // Begin the start sequence
        i2c_master_send(SLAVE_ADDR << 1);       // send the slave address, left shifted by 1, 
                                                // which clears bit 0, indicating a write
        i2c_master_send(master_write0);         // send a byte to the slave       
        i2c_master_send(master_write1);         // send another byte to the slave
        i2c_master_restart();                   // send a RESTART so we can begin reading 
        i2c_master_send((SLAVE_ADDR << 1) | 1); // send slave address, left shifted by 1,
                                                // and then a 1 in lsb, indicating read
        master_read0 = i2c_master_recv();       // receive a byte from the bus
        i2c_master_ack(0);                      // send ACK (0): master wants another byte!
        master_read1 = i2c_master_recv();       // receive another byte from the bus
        i2c_master_ack(1);                      // send NACK (1):  master needs no more bytes
        i2c_master_stop();                      // send STOP:  end transmission, give up bus

        ++master_write0;                        // change the data the master sends
        ++master_write1;
        
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