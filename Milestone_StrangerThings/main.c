#include <msp430G2553.h>


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	return 0;
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){

    switch(byte){
    case 0:

        break
    case 1:

        break;
    case 2:

        break;
    case 3:

        break;
    default:

        break;
    }
}
