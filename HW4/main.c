#include "NU32.h" // constants, funcs for startup and UART
#include "INIT_COM.h" // initializes SPI and I2C

void setVoltage(char channel, char voltage){

}

int main(void) {

  while(1) {
    CS = 0;                                          // check if SPI is working
    spi_io(0x5);                                     // ram read status command
    status = spi_io(0);                              // the actual status
    CS = 1;
  }
  return 0;
}