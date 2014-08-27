/*
 * Capcitive touch
 */

#include <msp430g2553.h>
#include "uart.h"
unsigned int ADC_value = 0;
unsigned char delimeter = 0xFF;

// function prototype
void ConfigureTimerA(void);
void ConfigureADC(void);

void uart_rx_isr(unsigned char c) {
	uart_putc(c);
	P1OUT ^= BIT0;		// toggle P1.0 (red led)
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	BCSCTL2 &= ~(DIVS_0);		// SMCLK = DCO = 1MHZ

	P1DIR  = BIT0 + BIT6; 		// P1.0 and P1.6 are the red+green LEDs
	P1OUT  = BIT0 + BIT6; 		// All LEDs off

	P1SEL |= BIT3;				// ADCinput pin P1.3

    uart_init();
    uart_set_rx_isr_ptr(uart_rx_isr);// register ISR called when data was received

	ConfigureADC();
	ConfigureTimerA();

    __enable_interrupt();

    for(;;){

    }
}

// ADC10 interrupt service routine
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void){
	ADC_value = ADC10MEM;		// Assigns the value held in ADC10MEM
	uart_putc(delimeter);
	uart_putint(ADC_value);
	__bic_SR_register_on_exit(CPUOFF);	// Return to activate mode
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void){
	LPM0_EXIT;
	P1OUT ^= BIT0;
	ADC10CTL0 |= ENC + ADC10SC; 	// Sampling/Conversion start
}

// Timer set-up
void ConfigureTimerA(void){
	TACCR0 = 30;
	TACCTL0 = CCIE;	// TACCR0 interrupt enabled
	TACTL = TASSEL_2 + MC_1;	// SMCLK, up to CCR0
	TACCR0 = 4000 -1;
	TACCTL1 = OUTMOD_3;	// TACCR1 set/reset
	TACCR1 = 3900;		// TACCR1 OUT1 ontime
}

// ADC set-up
void ConfigureADC(void){
	ADC10CTL1 = SHS_1 + CONSEQ_2 + INCH_3 + ADC10DIV_0;	// Channel 3, ADC10CLK, Repeat single channel, initiated by TimerA Output Unit 1
	ADC10CTL1 &= ~ADC10DF; // Binary format
	ADC10CTL1 |= ADC10SSEL_3;		// ADC10CLK = SMCLK
	ADC10CTL0 = SREF_0 + ADC10SHT_0 + ADC10ON + ADC10IE;	// Vcc & Vss as reference, Sample and hold for 64 ADC Clock cycles, ADC on, ADC interrupt enable
	ADC10AE0 |= BIT3;				// ADC input enable P1.3
}
