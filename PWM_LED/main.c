#include <msp430g2553.h>

int pwmDirection = 1;
/*
 * main.c
 */
int main(void) {
	WDTCTL = WDT_MDLY_32;		// Watchdog timer ~ 32ms
	IE1 |= WDTIE;				// enable watchdog timer interrupt
    P1DIR |= BIT6;				// Green LED as output
    P1SEL |= BIT6;				// Green LED controlled by Pulse width modulation
    TA0CCR0 = 1000;				// PWM period
    TA0CCR1 = 1;				// PWM duty cycle, time cycle on vs.off
    TA0CCTL1 = OUTMOD_7;		// TA0CCR1 reset/set -- high voltage below count and low voltage when past
    TA0CTL = TASSEL_2 + MC_1;	// Timer A control set to SMCLK clock TASSEL_2, 1MHz and count up mode MC_1
    _BIS_SR(LPM0_bits + GIE);
	return 0;
}

#pragma vector=WDT_VECTOR			// Watchdog Timer interrupt service routine
__interrupt void watchdog_timer(void){
	TA0CCR1 += pwmDirection * 20;	// Increase/ decrease duty cycle, on vs. off time
	if(TA0CCR1 > 980 || TA0CCR1 < 20){
		pwmDirection = -pwmDirection;
	}
}
