#include <xc.h>       
#include <stdio.h>
#include <math.h>
// DEVCFG0 (data sheet page 241-242)
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1 (data sheet page 243-244)
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
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

// Demonstrates spi
// PIC is the master, MCP4092 is the slave
// SDO1 -> SDI (pin A1 -> pin 5)
// SDI1 -> SO (pin A1 -> none)
// SCK1 -> SCK (pin B14 -> pin 4)
// SS1 -> CS (pin A4 -> pin 3)

// Additional SRAM connections
// Vss (Pin 12) -> ground
// Vdd (Pin 1) -> 3.3 V
// 
// Only uses the SRAM's sequential mode
//
#define CS LATAbits.LATA4      			    // chip select pin

void initSPI1();
unsigned char SPI1_IO(unsigned char);
/*void makeWaveformA();
void makeWaveformB();*/
void setVoltage(unsigned char, unsigned char); // int int

// initialize spi1
void initSPI1() {
  // set up the chip select pin as an output
  TRISAbits.TRISA4 = 0;
  CS = 1;
  RPA1Rbits.RPA1R = 0b0011;     // SDO1 pin

  // Master - SPI1, pins are: SDI1(A1), SDO1(A4), SCK1(B14).  
 
  // setup SPI1
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  //SPI1CONbits.MODE32 = 0;   // send 8 bits of data per transfer
  //SPI1CONbits.MODE16 = 0;   // send 8 bits of data per transfer
  SPI1BRG = 0x3E8;         // baud rate to 1 kHz [SPI4BRG = (50000000/(2*1000))-1 = 24999]
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on spi 1
}

// send a byte via spi and return the response
unsigned char SPI1_IO(unsigned char sent_data) {
  unsigned char received_data;
  SPI1BUF = sent_data;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}


void setVoltage(unsigned char channel, unsigned char voltage) {
// where channel is 0 or 1 (for VoutA and VoutB), and voltage is the 8-bit output level.
unsigned char byte1, byte2;

byte1 = (channel<<7);
byte1 = byte1 | (0b1110000);
byte1 = byte1 | (voltage >> 4);
byte2 = (voltage << 4);

CS=0;
SPI1_IO(byte1);
SPI1_IO(byte2);
CS=1;
}

int main(void) {
    
     __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    __builtin_enable_interrupts();
    
  unsigned int i = 0;
  unsigned char WaveformA[100];
  unsigned char WaveformB[100];
  double temp;
  
  for (i=0; i<100; i++){
      temp = 255.0/2.0 + (255.0/2.0)*sin((2.0*3.14159/100.0)*i);
      WaveformA[i] = (unsigned char) temp;
      temp = 2.55*i;
      WaveformB[i] = (unsigned char) temp;
  }

  initSPI1();
  i = 0;
  while(1) {
	  _CP0_SET_COUNT(0);
		while((_CP0_GET_COUNT()) < 48000000/2000){} // 1 ms delay
		setVoltage(0,WaveformA[i]);
        setVoltage(1,WaveformB[i]);
         
        i++;
        if(i==100){
            i=0;
        }
	  }
}