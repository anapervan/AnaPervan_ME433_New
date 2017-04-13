#include <stdio.h>
#include <stdlib.h>
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

// DEVCFG0 (data sheet page 241-242)
#pragma config DEBUG = 11 // no debugging
#pragma config JTAGEN = 0 // no jtag
#pragma config ICESEL = 11 // use PGED1 and PGEC1
#pragma config PWP = 111111111 // no write protect
#pragma config BWP = 0 // no boot write protect
#pragma config CP = 1 // no code protect

// DEVCFG1 (data sheet page 243-244)
#pragma config FNOSC = 011 // use primary oscillator with pll
#pragma config FSOSCEN = 0 // turn off secondary oscillator
#pragma config IESO = 0 // no switching clocks
#pragma config POSCMOD = 10 // high speed crystal mode
#pragma config OSCIOFNC = 1 // free up secondary osc pins
#pragma config FPBDIV = 00 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = 11 // do not enable clock switch
#pragma config WDTPS = 10100 // slowest wdt
#pragma config WINDIS = 1 // no wdt window
#pragma config FWDTEN = 0 // wdt off by default
#pragma config FWDTWINSZ = 11 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz (data sheet page 245-246)
#pragma config FPLLIDIV = 001 // divide input clock to be in range 4-5MHz (starts at 8, divide by 2)
#pragma config FPLLMUL = 111 // multiply clock after FPLLIDIV (multiply by 24 to get 96)
#pragma config FPLLODIV = 001 // divide clock after FPLLMUL to get 48MHz (divide by 2 to get 48)
#pragma config UPLLIDIV = 001 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB (divide by 2?)
#pragma config UPLLEN = 0 // USB clock on

// DEVCFG3 (data sheet page 247)
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = 0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 1 // USB BUSON controlled by USB module

#define SLAVE_ADDR = 0b0100000 // slave address

void initExpander(void)
void setExpander(char pin, char level)
unsigned char getExpander(void)

void initExpander(){
i2c_master_start();
i2c_master_send(SLAVE_ADDR << 1 | 0 );
i2c_master_send(0);
i2c_master_send(0b00001111);
i2c_master_stop();

i2c_master_start();
i2c_master_send(SLAVE_ADDR << 1 | 0);
i2c_master_send(0x6);
i2c_master_send(0b00001111);
i2c_master_stop();
}

unsigned char getExpander(){
i2c_master_start();
i2c_master_send(SLAVE_ADDR << 1 | 0);
i2c_master_send(0x9);
i2c_master_restart();
i2c_master_send(SLAVE_ADDR << 1 | 1);
unsigned char a = 0;
a = i2c_master_recv();
i2c_master_ack(1);
i2c_master_stop();
return a;
}

void setExpander(char pin, char level){
i2c_master_start();
i2c_master_send(SLAVE_ADDR << 1 | 0);
i2c_master_send(0x09);
i2c_master_send(0b00000001);
i2c_master_stop();
}

int main(int argc, char** argv) {

      Startup(); 
  __builtin_disable_interrupts();
  i2c_slave_setup(SLAVE_ADDR);              // init I2C5, which we use as a slave 
                                            //  (comment out if slave is on another pic)
  i2c_master_setup();                       // init I2C2, which we use as a master
  __builtin_enable_interrupts();
    
    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISBbits.TRISB4 = 1; // make push button an input
    TRISAbits.TRISA4 = 0; // make green LED an output
    LATAbits.LATA4 = 1; // set green LED initially high

    __builtin_enable_interrupts();

    ANSELBbits.ANSB2 = 0; // set as digital
    ANSELBbits.ANSB3 = 0; // set as digital
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    I2C2BRG = 223;
    I2C2CONBITS.ON=1;
    
    while(1) {
      
        if (r==(0b10000000)){LATAbits.LATA4=0;}
      else {LATAbits.LATA4=1;}
      _CP0_SET_COUNT(0);
      while (_CP0_GET_COUNT()<48000000/2000){}; // delay 1 ms
}

