#include <msp430.h> 

unsigned int i = 0;

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    // To set a specific pin as output or input, we write a '1' or '0' on the appropriate bit of the register.


        // P1DIR = <PIN7><PIN6><PIN5><PIN4><PIN3><PIN2><PIN1><PIN0>


        // Since we want to blink the on-board red LED, we want to set the direction of Port 1, Pin 0 (P1.0) as an output

        // We do that by writing a 1 on the PIN0 bit of the P1DIR register
        // P1DIR = <PIN7><PIN6><PIN5><PIN4><PIN3><PIN2><PIN1><PIN0>
        // P1DIR = 0000 0001
        // P1DIR = 0x01     <-- this is the hexadecimal conversion of 0000 0001

    P1DIR |= 0x41;
    P1OUT |= 0x01;
    P1OUT &= ~0x40;

    for(;;){
    	   // Toggle P1.0 using exclusive-OR operation (^=)

    	    // P1OUT is another register which holds the status of the LED.
    	    // '1' specifies that it's ON or HIGH, while '0' specifies that it's OFF or LOW
    	    // Since our LED is tied to P1.0, we will toggle the 0 bit of the P1OUT register
    	P1OUT ^= 0x41;
    	for(i=0; i< 20000; i++);
    }
	return 0;
}
