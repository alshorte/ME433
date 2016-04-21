#ifndef INIT_COM_H
#define INIT_COM_H

#define CS LATBbits.LATB15 // chip select pin
#define SLAVE_ADDR 0x32 // slave address for i2c

// spi function prototypes
void spi_init();
unsigned char spi_io(unsigned char o);
// i2c function prototypes
void i2c_init();
void expander_init();
void i2c_master_setup(void);
void i2c_master_start(void);
void i2c_master_restart(void);
void i2c_master_send(unsigned char byte);
unsigned char i2c_master_recv(void);
void i2c_master_ack(int val);
void i2c_master_stop(void);

#endif

