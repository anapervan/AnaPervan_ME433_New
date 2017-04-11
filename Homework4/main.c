#include "NU32.h"       // constants, funcs for startup and UART
#include <stdio.h>
#include <math.h>
// Demonstrates spi
// PIC is the master, MCP4092 is the slave
// SDO1 -> SDI (pin A4 -> pin 5)
// SDI1 -> SO (pin A1 -> none)
// SCK1 -> SCK (pin B14 -> pin 4)
// SS1 -> CS (pin B3 -> pin 3)

// Additional SRAM connections
// Vss (Pin 12) -> ground
// Vdd (Pin 1) -> 3.3 V
// 
// Only uses the SRAM's sequential mode
//
#define CS LATBbits.LATB3      			    // chip select pin
#define NUMSAMPS 100	   					// number of points in waveform
float WaveformA[100], WaveformB[100];

// initialize spi1
void initSPI1() {
  // set up the chip select pin as an output
  // when a command is beginning (clear CS to low) and when it is ending (set CS high)
  TRISBbits.TRISB3 = 0;
  CS = 1;

  // Master - SPI1, pins are: SDI1(A1), SDO1(A4), SCK1(B14).  
  // we manually control SS1 as a digital output (B3)
  // since the pic is just starting, we know that spi is off. We rely on defaults here
 
  // setup SPI1
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1CONbits.MODE32 = 0;   // send 8 bits of data per transfer
  SPI1CONbits.MODE16 = 0;   // send 8 bits of data per transfer
  SPI1BRG = 0x61A7;         // baud rate to 1 kHz [SPI4BRG = (50000000/(2*1000))-1 = 24999]
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on spi 1
}

// send a byte via spi and return the response
void SPI1_IO(unsigned char sent_data) {
  SPI1BUF = sent_data;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  received_data = SPI1BUF;
}

void makeWaveformA() {
	int i = 0;								// sine wave
	for (i = 0; i < NUMSAMPS; ++i) {
			WaveformA[i] = 255*sin((2*3.14159/100)*i);;
		}
	}
}

void makeWaveformB() {
	int i = 0;		// square wave, fill in center value and amplitude
	for (i = 0; i < NUMSAMPS; ++i) {
		WaveformB[i] = 2.55*i;
	}
}

void setVoltage(char channel, char voltage) {
// where channel is 0 or 1 (for VoutA and VoutB), and voltage is the 8-bit output level.
unsigned char byte1, byte2, output;
if (channel == 0) {
	byte1 = (voltage >> 4) + 0b01110000 // right shift voltage 4 spaces and add 0111 at the beginning	
}
if (channel == 1) {
	byte1 = (voltage >> 4) + 0b11110000 // right shift voltage 4 spaces and add 1111 at the beginning
}
byte2 = (voltage << 4); 			// left shift voltage 4 spaces

SPI1_IO(byte1);
SPI1_IO(byte2);
}

int main(void) {
  RPA4Rbits.RPA4R = 0b0011;
  
  while(1) {
	  for (i = 0; i < NUMSAMPS; ++i) {
		setVoltage(0,WaveformA[i])
		setVoltage(1,WaveformB[i])
	  
	  _CP0_SET_COUNT(0)
		while((_CP0_GET_COUNT()) < 40){
			; // delay
		}
		
	  }
  }
  return 0;
}