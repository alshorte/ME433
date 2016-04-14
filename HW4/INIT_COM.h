#ifndef INIT_COM_H
#define INIT_COM_H

#define CS LATBbits.LATB8 // chip select pin --> change to the appropriate bit

void spi_init();
void i2c_init();


#endif