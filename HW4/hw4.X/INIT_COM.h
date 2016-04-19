#ifndef INIT_COM_H
#define INIT_COM_H

#define CS LATBbits.LATB15 // chip select pin

void spi_init();
void i2c_init();
void expander_init();
unsigned char spi_io(unsigned char o);


#endif

