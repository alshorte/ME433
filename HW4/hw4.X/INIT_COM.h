#ifndef INIT_COM_H
#define INIT_COM_H

#define CS LATBbits.LATB15 // chip select pin
#define SLAVE_ADDR 0b0100000 // slave address for i2c2
#define SLAVE_ADDR2 0b1101011//slave address for imu

// spi function prototypes
void spi_init(void);
unsigned char spi_io(unsigned char o);
// i2c function prototypes
void i2c_init(void);
void i2c_master_start(void);
void i2c_master_restart(void);
void i2c_master_send(unsigned char byte);
unsigned char i2c_master_recv(void);
void i2c_master_ack(int val);
void i2c_master_stop(void);
void i2c_write(unsigned char reg, unsigned char val, unsigned char ADD);
unsigned char i2c_read(unsigned char reg, unsigned char ADD);
void expander_init(void);
#endif

