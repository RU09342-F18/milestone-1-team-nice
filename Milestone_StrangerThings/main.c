#include <msp430G2553.h>


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	P1OUT = 0x00;                           // Initialize all GPIO
	P1DIR = 0xFF & ~UART_RXD;               // Set all pins but RXD to output

	return 0;
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){

    switch(byte){
    case 0:     //calculate and send Length Byte

        break
    case 1:     //set Red LED PWM

        break;
    case 2:     //set Green LED PWm

        break;
    case 3:     //set Blue LED PWM

        break;
    default:    //Send the rest of the data to next node

        break;
    }
}
