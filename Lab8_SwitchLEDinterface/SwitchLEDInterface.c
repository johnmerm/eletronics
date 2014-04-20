// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 11, 2014

// Lab 8
//      Jon Valvano and Ramesh Yerraballi
//      November 21, 2013

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

#include <stdarg.h>



void init_dig(char port,int numOfPins, ...){//arargs are pin,isOutput
	int i=0;
	
	va_list pins;
	volatile unsigned long *dirReg,*afselReg,*amselReg,*denReg,*pctlReg;
	
	
	if (port == 'A'){
		dirReg = &GPIO_PORTA_DIR_R;
		afselReg = &GPIO_PORTA_AFSEL_R;
		amselReg = &GPIO_PORTA_AMSEL_R;
		denReg = &GPIO_PORTA_DEN_R;
		pctlReg = &GPIO_PORTA_PCTL_R;
	}else if (port == 'B'){
		dirReg = &GPIO_PORTB_DIR_R;
		afselReg = &GPIO_PORTB_AFSEL_R;
		amselReg = &GPIO_PORTB_AMSEL_R;
		denReg = &GPIO_PORTB_DEN_R;
		pctlReg = &GPIO_PORTB_PCTL_R;
	}else if (port == 'C'){
		dirReg = &GPIO_PORTC_DIR_R;
		afselReg = &GPIO_PORTC_AFSEL_R;
		amselReg = &GPIO_PORTC_AMSEL_R;
		denReg = &GPIO_PORTC_DEN_R;
		pctlReg = &GPIO_PORTC_PCTL_R;
	}else if (port == 'D'){
		dirReg = &GPIO_PORTD_DIR_R;
		afselReg = &GPIO_PORTD_AFSEL_R;
		amselReg = &GPIO_PORTD_AMSEL_R;
		denReg = &GPIO_PORTD_DEN_R;
		pctlReg = &GPIO_PORTD_PCTL_R;
	}else if (port == 'E'){
		dirReg = &GPIO_PORTE_DIR_R;
		afselReg = &GPIO_PORTE_AFSEL_R;
		amselReg = &GPIO_PORTE_AMSEL_R;
		denReg = &GPIO_PORTE_DEN_R;
		pctlReg = &GPIO_PORTE_PCTL_R;

	}else if (port == 'F'){
		dirReg = &GPIO_PORTF_DIR_R;
		afselReg = &GPIO_PORTF_AFSEL_R;
		amselReg = &GPIO_PORTF_AMSEL_R;
		denReg = &GPIO_PORTF_DEN_R;
		pctlReg = &GPIO_PORTF_PCTL_R;

	}
	
	va_start(pins,numOfPins);
	for (i=0;i<numOfPins;i++){
		int pin = va_arg(pins,int);
		int isOutput = va_arg(pins,int);
		*denReg |= (1<<pin);
		*amselReg &= ~(1<<pin);
		*afselReg &= ~(1<<pin);
		*pctlReg &= ~(0xF<<(4*pin));
		
		if (isOutput == 1){
			*dirReg |= (1<<pin);
		}else{
			*dirReg &= ~(1<<pin);
		}
	}
	va_end(pins);
}


unsigned long read_pin(volatile unsigned long *reg,int pin){
	
	unsigned long bitmask = 1<<pin;
	unsigned long value = *reg & bitmask;
	return value;
}

void set_pin(volatile unsigned long *reg,int pin){
	*reg |= (1<<pin);
}

void unset_pin(volatile unsigned long *reg,int pin){
	*reg &= ~(1<<pin);
}

void toggle_pin(volatile unsigned long *reg,int pin){
	*reg ^= (1<<pin);
}


// Subroutine to delay in units of half seconds
// Inputs: Number of half seconds to delay
// Outputs: None
// simple delay function
// which delays time*500 milliseconds
// assuming 80 MHz clock
void delay(unsigned long hund_millis){
  unsigned long i;
  while(hund_millis > 0){
    i = 1333333;
    while(i > 0){
      i = i - 1;
    }
    hund_millis = hund_millis - 1;
  }
}

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10 k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).

int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
	unsigned long volatile v;
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1);  // activate grader and set system clock to 80 MHz
  
	//
	
	SYSCTL_RCGC2_R |= 0x10;           // Port E clock
  v = SYSCTL_RCGC2_R;           // wait 3-5 bus cycles
	
	init_dig('E',2,0,0,1,1); //E0 input E1 output
	/*
	GPIO_PORTE_AFSEL_R &= ~0x03;
	GPIO_PORTE_AMSEL_R &= ~0x03;
	GPIO_PORTE_DEN_R |= 0x03;
	GPIO_PORTE_PCTL_R &= ~0xFF;
	GPIO_PORTE_DIR_R |= 0x02;
	GPIO_PORTE_DIR_R &= ~0x01;
	*/
	
	set_pin(&GPIO_PORTE_DATA_R,1);
	//GPIO_PORTE_DATA_R |= 0x02;
	
  EnableInterrupts();           // enable interrupts for the grader
  while(1){
		//unsigned long bv = GPIO_PORTE_DATA_R&0x01;
		unsigned long bv = read_pin(&GPIO_PORTE_DATA_R,0);
		if (bv){
			//GPIO_PORTE_DATA_R ^= 0x02;
			toggle_pin(&GPIO_PORTE_DATA_R,1);
		}else{
			set_pin(&GPIO_PORTE_DATA_R,1);
		}
		delay(1);
  }
  
}
