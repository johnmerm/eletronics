// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// March 13, 2014
// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "..//tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){
	GPIO_PORTB_DIR_R 		|= 0x1F;  		// make PB0-4 output
	GPIO_PORTB_AFSEL_R 	&= ~0x1F;			// disable alt funct on PB0-4
	GPIO_PORTB_AMSEL_R 	&= ~0x1F;   	// disable analog functionality on PB0-4
  GPIO_PORTB_DEN_R 		|= 0x1F;   		// enable digital I/O on PB0-4
	GPIO_PORTB_DR8R_R		|= 0x1F;   		// enable 8mA  on PB0-4
	GPIO_PORTB_PCTL_R 	&= 0xFFFF0000;// configure PB0-4 as GPIO
}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
  GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R & ~0xF) | (0xF & data);
}
