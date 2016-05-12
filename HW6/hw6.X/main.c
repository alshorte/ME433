#include <xc.h>           // processor SFR definitions
#include "INIT_COM.h" // initializes SPI and I2C
#include "DEV_CONFIG.h" //configure pic
#include "LCD.h" // lcd draw functions
#include "ILI9163C.h"       //LCD functions

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
//void oc_init(void); 



int main(void){
    unsigned char whoami_status;  // initialize variable for i2c read
    char msg[100];      // initialize string array
    short temp, gyroX, gyroY, gyroZ, accX, accY, accZ;     // initialize variables to store data read via I2C
    
    __builtin_disable_interrupts();

      __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

      // 0 data RAM access wait states
      BMXCONbits.BMXWSDRM = 0x0;

      // enable multi vector interrupts
      INTCONbits.MVEC = 0x1;

      // disable JTAG to get pins back
      DDPCONbits.JTAGEN = 0;
      
      //oc_init();    // initialize output compare pins while interrupts disabled

      __builtin_enable_interrupts();

    i2c2_init();              // initialize I2C2
    IMU_init();                // turn on accelerometer and gyro
    SPI1_init();              // initialize SPI and LCD
    LCD_init();
    LCD_clearScreen(WHITE);   // clear screen to white
         
    while(1){
          
        // initialize x, y position to write to
        char x = 28;
        char y = 32;
        
        unsigned char reg;
        reg = 0x0F; //WHOAMI register
        whoami_status = i2c_read(reg, SLAVE_ADDR2);    // check if who am I is 0b01101001
        sprintf(msg,"WhoAmI: %d",whoami_status);
        draw_string(msg, x, y);           // send whoami_status

        y = y + 8;    // shift down a line
          /*
          if(whoami_status == 0b01101001){ // whoami is expected value 
                CS = 1; // set CS to high to turn on LED
            }
           */
         
         // _CP0_SET_COUNT(0);   // set core timer to 0
        temp = i2c_IMUread(regTL, regTH);     // read temperature of LSM6DS33
        gyroX = i2c_IMUread(regGXL, regGXH);  // read pitch angular rate of LSM6DS33
        gyroY = i2c_IMUread(regGYL, regGYH);  // read roll angular rate of LSM6DS33
        gyroZ = i2c_IMUread(regGZL, regGZH);  // read yaw angular rate of LSM6DS33
        accX = i2c_IMUread(regAXL, regAXH);   // read X acceleration of LSM6DS33
        accY = i2c_IMUread(regAYL, regAYH);   // read Y acceleration of LSM6DS33
        accZ = i2c_IMUread(regAZL, regAZH);   // read Z acceleration of LSM6DS33
        
        sprintf(msg,"Temperature: %d",temp);
        draw_string(msg, x, y);           // send temperature reading
        y = y + 8;    // shift down a line

        sprintf(msg,"X Acceleration: %d", accX);
        draw_string(msg, x, y);           // send accX reading
        y = y + 8;    // shift down a line

        sprintf(msg,"Y Acceleration: %d", accY);
        draw_string(msg, x, y);           // send accY reading 
        y = y + 8;    // shift down a line

        sprintf(msg,"Z Acceleration: %d", accZ);
        draw_string(msg, x, y);           // send accZ reading 

            /* output the X acceleration on OC1 and the Y acceleration on OC2 --> get rid of this, send to LCD
            float tempX, tempY;
            tempX = (accX/32000)*PR2; // scale to max size of duty cycle
            tempY = (accY/32000)*PR2;
            OC1RS = tempX;
            OC2RS = tempY;
            while (_CP0_GET_COUNT() < 480080){ // running at 48 MHz, Core timer at 24 MHZ, delay 20 ms --> read every 50Hz
                ;
            }
           */
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
    



