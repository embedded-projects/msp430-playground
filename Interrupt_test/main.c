#include <msp430g2553.h>

unsigned int i = 0;
/*
 * main.c
 */
void main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    P1DIR |= BIT0; 	// Set P1.0 to output direction
    P1REN |= BIT3;
    P1OUT |= BIT0;

    P1OUT |= BIT3;	// Select pull-up mode for P1.3
    P1IES |= BIT3;	// P1.3 hi/lo edge
    P1IE |= BIT3;	// P1.3 interrupt enabled
    P1IFG &= ~BIT3;	//P1.3 IFG cleared

    _BIS_SR( LPM4_bits + GIE);	// Enter LPM4 w/interrupt
    for(;;){
    }
}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	P1OUT ^= BIT0;
	for(i=0;i<60000;i++);
	P1OUT ^= BIT0;
	P1IFG &= ~BIT3;	// P1.3 IFG cleared
}
