#include<xc.h>           // processor SFR definitions
#include "INIT_COM.h"

//******************************************************************//
//SPI SETUP AND FUNCTIONS
// initialize spi4 and the ram module --> edit as apropriate for our case
void spi1_init() {
  
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

// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;                  //
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}


//*********************************************************************//
// I2C SETUP AND FUNCTIONS
void i2c2_init(){
  // some initialization function to set the right speed setting
   I2C2BRG = 233; // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 
                                    //FSCK = 100 kHz (or 400kHz) PGD = 104 ns PBLCK = 48MHz
  I2C2CONbits.ON = 1;               // turn on the I2C2 module

  // initialize PIC pins as I2C
  ANSELBbits.ANSB2 = 0; // Turn off analog on B2 so it can be used for SDA2
  ANSELBbits.ANSB3 = 0; // turn off analog on B3 so it can be used for SCL2
  

}

// general i2c send/receive functions
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}

// i2C functions for our code
void i2c_write(unsigned char reg, unsigned char data){ // should these be chars????
    i2c_master_start();                     // Begin the start sequence
    i2c_master_send(SLAVE_ADDR << 1);       // send the slave address, left shifted by 1, 
    i2c_master_send(reg);       // register you want to change
    i2c_master_send(data);         // what you want it to be
    i2c_master_stop();
}

unsigned char i2c_read(unsigned char reg){ 
    i2c_master_start();                     // Begin the start sequence
    i2c_master_send(SLAVE_ADDR << 1);       // send the slave address, left shifted by 1, 
    i2c_master_send(reg);       // register you want to change
    i2c_master_restart();
    i2c_master_send(SLAVE_ADDR << 1 |1);       // send the slave address, left shifted by 1, 
    i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
}

void expander_init(){
    // initialize pins G0 - G3  as outputs
    // initialize pins G4 - G7 as inputs
    unsigned char ex_val = 0b11110000;
    unsigned char reg = 0; // IODIR register sets pins to me inputs or outputs
    i2c_write(reg, ex_val); // send desired expander values to set input and ouput pins via IODIR
}


