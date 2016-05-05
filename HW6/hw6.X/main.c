#include <xc.h>           // processor SFR definitions
#include "INIT_COM.h" // initializes SPI and I2C
//#include "DEV_CONFIG.h" //configure pic
// DEVCFG0
#pragma config DEBUG = 1 // no debugging
#pragma config JTAGEN = 0 // no jtag
#pragma config ICESEL = 11 // use PGED1 and PGEC1
#pragma config PWP = 111111111 // no write protect
#pragma config BWP = 0 // no boot write protect
#pragma config CP = 1 // no code protect

// DEVCFG1
#pragma config FNOSC = 011 // use primary oscillator with pll
#pragma config FSOSCEN = 0 // turn off secondary oscillator
#pragma config IESO = 0 // no switching clocks
#pragma config POSCMOD = 10 // high speed crystal mode
#pragma config OSCIOFNC = 1  // free up secondary osc pins
#pragma config FPBDIV = 00 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = 10 // do not enable clock switch
#pragma config WDTPS = 10100 // slowest wdt
#pragma config WINDIS = 1 // no wdt window
#pragma config FWDTEN = 0 // wdt off by default
#pragma config FWDTWINSZ = 11 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = 001 // divide input clock to be in range 4-5MHz (8 MHz clock -> divde by 2)
#pragma config FPLLMUL = 111 // multiply clock after FPLLIDIV (24x multiplier -> 96 MHz)
#pragma config FPLLODIV = 001 // divide clock after FPLLMUL to get 48MHz (2x divider -> 48 MHz)
#pragma config UPLLIDIV = 001 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = 0 // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = 0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 1 // USB BUSON controlled by USB module


// registers  to read from for LSM6DS33
#define regTL 0x20     // temp output register. L and H express 16 bit word in 2s comp
#define regTH 0x21
#define regGXL 0x22    // pitch axis angular rate output register. L and H express 16 bit word in 2s comp
#define regGXH 0x23
#define regGYL 0x24    // roll axis angular rate output register. L and H express 16 bit word in 2s comp
#define regGYH 0x25
#define regGZL 0x26    // yaw axis angular rate output register. L and H express 16 bit word in 2s comp
#define regGZH 0x27
#define regAXL 0x28    // X linear acceleration output register. L and H express 16 bit word in 2s comp
#define regAXH 0x29
#define regAYL 0x2A    // Y linear acceleration output register. L and H express 16 bit word in 2s comp
#define regAYH 0x2B
#define regAZL 0x2C    // Z linear acceleration output register. L and H express 16 bit word in 2s comp
#define regAZH 0x2D

// function prototypes
void oc_init(void); 



int main(void){
    unsigned char whoami_status;  // initialize variable for i2c read
    unsigned short temp, gyroX, gyroY, gyroZ, accX, accY, accZ;     // initialize variables to store data read via I2C
    
    __builtin_disable_interrupts();

      __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

      // 0 data RAM access wait states
      BMXCONbits.BMXWSDRM = 0x0;

      // enable multi vector interrupts
      INTCONbits.MVEC = 0x1;

      // disable JTAG to get pins back
      DDPCONbits.JTAGEN = 0;
      
      oc_init();    // initialize output compare pins while interrupts disabled

      __builtin_enable_interrupts();

      i2c2_init();  // initialize I2C
      spi1_init(); // not actually using, I just connected the CS to an LED to test is WHOAMI is working

      while(1){
          
          unsigned char reg;
          reg = 0x0F; //WHOAMI register
          whoami_status = i2c_read(reg); // check if who am I is 0b01101001
          
          if(whoami_status == 0b01101001){ // whoami is expected value 
                CS = 1; // set CS to high to turn on LED
            }
          
          
          _CP0_SET_COUNT(0);   // set core timer to 0
            temp = i2c_IMUread(regTL, regTH);     // read temperature of LSM6DS33
            gyroX = i2c_IMUread(regGXL, regGXH);  // read pitch angular rate of LSM6DS33
            gyroY = i2c_IMUread(regGYL, regGYH);  // read roll angular rate of LSM6DS33
            gyroZ = i2c_IMUread(regGZL, regGZH);  // read yaw angular rate of LSM6DS33
            accX = i2c_IMUread(regAXL, regAXH);   // read X acceleration of LSM6DS33
            accY = i2c_IMUread(regAYL, regAYH);   // read Y acceleration of LSM6DS33
            accZ = i2c_IMUread(regAZL, regAZH);   // read Z acceleration of LSM6DS33

            // output the X acceleration on OC1 and the Y acceleration on OC2
            float tempX, tempY;
            tempX = (accX/32000)*PR2; // scale to max size of duty cycle
            tempY = (accY/32000)*PR2;
            OC1RS = tempX;
            OC2RS = tempY;
            while (_CP0_GET_COUNT() < 480080){ // running at 48 MHz, Core timer at 24 MHZ, delay 20 ms --> read every 50Hz
                ;
            }
           
      }
      return;
}

void oc_init(){
    //Set up pins for OC1 and OC2, to output PWM at 1kHz using timer2, with a PR2 value between 1000 and 10000
    //Initialize the duty cycle to 50%.
    // OC1 can be pins A0 B3 B4 B15 B7 
    // OC2 can be pins A1 B5 B1 B11 B8 A8 A9
    // set pin XX to be OC1 and pin XX to be OC2
    ANSELAbits.ANSA0 = 0;       // Turn off analog on A0 so it can be used for OC1 --> need to change my PWM pins
    ANSELAbits.ANSA1 = 0;       // turn off analog on A1 so it can be used for OC2
    RPA0Rbits.RPA0R = 0b0101;   //set OC1 to be pin A0
    RPA1Rbits.RPA1R = 0b0101;   //set OC2 to be pin A1
    
    // set up Timer and PWM
    T2CONbits.TCKPS = 4;        // Timer2 prescaler N=16 (1:16)
    PR2 = 4999;                 // period = (PR2+1) * N * 12.5 ns = 1ms, 1kHz
    TMR2 = 0;                   // initial TMR2 count is 0
    
    // set up OC1
    OC1CONbits.OCTSEL = 0;      // set timer 2 to output compare
    OC1CONbits.OCM = 0b110;     // PWM mode without fault pin; other OC1CON bits are defaults
    OC1RS = 2500;               // duty cycle = OC1RS/(PR2+1) = 50%
    OC1R = 2500;                // initialize before turning OC1 on; afterward it is read-only
    
    //set up OC2
    OC2CONbits.OCTSEL = 0;      // set timer 2 to output compare
    OC2CONbits.OCM = 0b110;     // PWM mode without fault pin; other OC1CON bits are defaults
    OC2RS = 2500;               // duty cycle = OC2RS/(PR2+1) = 50%
    OC2R = 2500;                // initialize before turning OC1 on; afterward it is read-only
    
    T2CONbits.ON = 1;           // turn on Timer2
    OC1CONbits.ON = 1;          // turn on OC1
    OC2CONbits.ON = 1;          // turn on OC2
}
    



