#include <msp430g2553.h>

#define RED BIT0
#define GREEN BIT6
#define BTN BIT3

void delay(void);

unsigned int i = 0;
/*
 * main.c
 */
void main(void) {
	unsigned int flash;
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    P1OUT = 0;
    P1DIR |= RED + GREEN; // LED pin to outputs, BTN is still an input by default

    P1REN |= BIT3;
    P1OUT |= BIT3;	// Select pull-up mode for P1.3

    for(;;){
    	for(flash = 0; flash < 8; flash ++){
    		P1OUT ^= RED;	// RED led on

        	for(i=0; i< 20000; i++);
    		//delay();		// call delay function
    		//P1OUT &= ~RED;	// red LED off
    		//delay();		// delay again
    	}
    	while((P1IN & BTN) !=0 ); // wait for button press

    	for(flash = 0; flash <8; flash ++){
    		P1OUT ^= GREEN;	// GREEN led on

        	for(i=0; i< 20000; i++);
    		//delay();		// call delay function
    		//P1OUT &= ~ GREEN;	// GREEN LED off
    		//delay();		// delay again
    	}

    	while((P1IN & BTN) !=0 ); // wait for button press
    }
}

void delay(void){
	unsigned int count;
	for(count = 0; count < 60000; count ++ );
}
