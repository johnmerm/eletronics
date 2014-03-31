// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// November 7, 2013

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// ***** 3. Subroutines Section *****

#define MASK_PORTB 0x3F 							// Pins 0,1,2,3,4,5#define MASK_PORTE 0x07								// Pins 0,1,2#define MASK_PORTF 0x0A								// Pins 1,3
// configure the system to get its clock from the PLL

void SysTick_Init(void) {
	NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
	NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}

void init() {
	volatile unsigned long delay;

	SYSCTL_RCGC2_R |= 0x32;           // Port B,E,F clock
	delay = SYSCTL_RCGC2_R;           // wait 3-5 bus cycles

	SysTick_Init();

	GPIO_PORTB_DIR_R |= MASK_PORTB;
	GPIO_PORTB_AFSEL_R &= ~MASK_PORTB;      	// not alternative
	GPIO_PORTB_AMSEL_R &= ~MASK_PORTB;      	// no analog
	GPIO_PORTB_PCTL_R &= ~0x00FFFFFF;
	GPIO_PORTB_DEN_R |= MASK_PORTB;

	GPIO_PORTE_DIR_R &= ~MASK_PORTE;       //PE0-2 Inputs
	GPIO_PORTE_AFSEL_R &= ~MASK_PORTE;
	GPIO_PORTE_AMSEL_R &= ~MASK_PORTE;      // no analog
	GPIO_PORTE_PCTL_R &= ~0x00000FFF; // bits for PD3, PD0
	GPIO_PORTE_DEN_R |= MASK_PORTE;         // enable PD3, PD0

	GPIO_PORTF_DIR_R |= MASK_PORTF;         // PD3 output
	GPIO_PORTF_AFSEL_R &= ~MASK_PORTF;      // not alternative
	GPIO_PORTF_AMSEL_R &= ~MASK_PORTF;      // no analog
	GPIO_PORTF_PCTL_R &= ~0x0000F0F0; // bits for PD3, PD0
	GPIO_PORTF_DEN_R |= MASK_PORTF;         // enable PD3, PD0
}
							/* PB5 PB4 PB3 PB2 PB1 PB0
								 RE  YE  GE  RN  YN  GN */
#define RR 0x24//	1		0		0		1		0		0#define GR 0x0C//	0		0		1		1		0		0#define YR 0x14//	0		1		0		1		0		0#define RG 0x21//	1		0		0		0		0		1#define RY 0x22//	1		0		0		0		1		0
								/* PF3 PF2 PF1 PF0 */
#define W		0x08//	1		0		0		0
#define S 	0x02//	0		0		1		0


// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay) {
	NVIC_ST_RELOAD_R = delay - 1;  // number of counts to wait
	NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
	while ((NVIC_ST_CTRL_R & 0x00010000) == 0) { // wait for count flag
	}
}
// 10000us equals 10ms
void SysTick_Wait10ms(unsigned long delay) {
	unsigned long i;
	for (i = 0; i < delay; i++) {
		SysTick_Wait(800000);  // wait 10ms
	}
}

struct State {
	unsigned long out;
	unsigned long out_walk;
	unsigned long time;
	int next[8];
};
typedef const struct State FSMType;
#define S_in 0 //All red#define S_ge 1 //East green#define S_ye 2 //East yellow#define S_wa 3 //Walk#define S_h0 4 //Hurry_0
#define S_h1 5 //Hurry_1
#define S_h2 6 //Hurry_2
#define S_h3 7 //Hurry_3#define S_gn 8 //North green#define S_yn 9 //North yellow
const FSMType FSM[10] = {
						/*	No		e			n			en		p			pe		pn		pne */
{ RR, S, 0,  { S_in, S_ge, S_gn, S_ge, S_wa, S_ge, S_gn, S_ge } }, //init
{ GR, S, 50, { S_ge, S_ge, S_ye, S_ye, S_ye, S_ye, S_ye, S_ye } }, //S_ge
{ YR, S, 10, { S_gn, S_ge, S_gn, S_gn, S_wa, S_wa, S_gn, S_gn } }, //S_ye - Moves to gn
{ RR, W, 50, { S_h0, S_h0, S_h0, S_h0, S_wa, S_h0, S_h0, S_h0 } }, //Walk
{ RR, S, 02, { S_h1, S_h1, S_h1, S_h1, S_h1, S_h1, S_h1, S_h1 } }, //Hurry0 - can only go to h1
{ RR, 0, 02, { S_h2, S_h2, S_h2, S_h2, S_h2, S_h2, S_h2, S_h2 } }, //Hurry1 - can only go to h2
{ RR, S, 02, { S_h3, S_h3, S_h3, S_h3, S_h3, S_h3, S_h3, S_h3 } }, //Hurry2 - can only go to h3
{ RR, 0, 10, { S_ge, S_ge, S_gn, S_ge, S_wa, S_ge, S_gn, S_ge } }, //Hurry3
		
{ RG, S, 50, { S_gn, S_yn, S_gn, S_yn, S_yn, S_yn, S_yn, S_yn } }, //North green
{ RY, S, 10, { S_wa, S_ge, S_gn, S_ge, S_wa, S_wa, S_wa, S_wa } } //North yellow - Move to wa
};

void set_lights_and_wait(int state) {
	FSMType fState = FSM[state];
	GPIO_PORTB_DATA_R = fState.out;
	GPIO_PORTF_DATA_R = fState.out_walk;
	SysTick_Wait10ms(FSM[state].time);
	
}

int main(void) {
	int input, state = S_in;
	TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); // activate grader and set system clock to 80 MHz
	init();
	EnableInterrupts();

	while (1) {
		set_lights_and_wait(state);
		input = GPIO_PORTE_DATA_R & MASK_PORTE;
		state = FSM[state].next[input];
	}
}

