#include <msp430g2553.h>

#define LED_0 BIT0
#define LED_1 BIT6
#define LED_OUT P1OUT
#define LED_DIR P1DIR

unsigned int timerCount = 0;
/*
 * main.c
 */
void main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    LED_DIR |= (LED_0 + LED_1);
    LED_OUT &= ~(LED_0 + LED_1);

    CCTL0 = CCIE;
    TACTL = TASSEL_2 + MC_2;		// Set the timer A to SMCLK, continuous
    // Clear the timer and enable timer interrupt

    __enable_interrupt();

    __bis_SR_register(LPM0 + GIE); // LPM0 with interrupts enabled
}

// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void){
	timerCount = (timerCount + 1) %8 ;
	if(timerCount == 0){
		P1OUT ^= (LED_0 + LED_1);
	}
}
