#include<xc.h>           // processor SFR definitions
#include "INIT_COM.h"


// initialize spi4 and the ram module --> edit as apropriate for our case
void spi_init() {
  
  // the chip select pin is used by the sram to indicate
  // when a command is beginning (clear CS to low) and when it
  // is ending (set CS high)
  ANSELBbits.ANSB15 = 0; // Turn off analog on B15 so it can be used for CS
  ANSELBbits.ANSB14 = 0; // turn off analog on B14 so it can be used for SCK1
  TRISBbits.TRISB15 = 0; // set up the chip select pin as an output
  CS = 1; // chip select set high (no command being issued atm)
  
  RPB13Rbits.RPB13R = 0b0011; //set SDO to be pin RPB13
  SDI1Rbits.SDI1R = 0b0100; // set SDI to be pin RPB8

  // Master - SPI1, pins are: SDI1(RPB8 - pin 17), SDO1(RPB13 - pin24), SCK1(pin 25).  
  // we manually control SS1 as a digital output (B15)
  // since the pic is just starting, we know that spi is off. We rely on defaults here
 
  // setup spi1
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1BRG = 300;              // baud rate to 12MHz [SPI1BRG = (48000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on spi 1

                            // send a ram set status command.
  CS = 0;                   // enable the ram
  spi_io(0x01);             // ram write status
  spi_io(0x41);             // sequential mode (mode = 0b01), hold disabled (hold = 0)
  CS = 1;                   // finish the command
}
/*
void i2c_init(){
	// some initialization function to set the right speed setting
  char buf[100] = {};                       // buffer for sending messages to the user
  unsigned char master_write0 = 0xCD;       // first byte that master writes
  unsigned char master_write1 = 0x91;       // second byte that master writes
  unsigned char master_read0  = 0x00;       // first received byte
  unsigned char master_read1  = 0x00;       // second received byte

  // some initialization function to set the right speed setting
  Startup(); 
  __builtin_disable_interrupts();
  i2c_slave_setup(SLAVE_ADDR);              // init I2C5, which we use as a slave 
                                            //  (comment out if slave is on another pic)
  i2c_master_setup();                       // init I2C2, which we use as a master
  __builtin_enable_interrupts();
}

void expander_init(){

}
 **/

// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;                  //
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}




