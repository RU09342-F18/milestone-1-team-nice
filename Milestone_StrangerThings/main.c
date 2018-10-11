#include <msp430g2553.h>

//Milestone 1

#define UART_RXD BIT0

void redPWM(int DC);
void greenPWM(int DC);
void bluePWM(int DC);

int byteNumber = 0;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    BCSCTL3 = LFXT1S_2;                     //interfaces with crystal (needed for clock to work)
    TA0CCR0 = 0xFFFF;   //set CCR0 to max possible value
    TA1CCR0 = 0xFFFF;   //set TA1's CCRO to max possible value


    P2DIR |= BIT1 + BIT3 + BIT5;
    P2SEL2 &= ~(BIT1 + BIT3 + BIT5);
    P2SEL |= BIT1 + BIT3 + BIT5;

	P1OUT = 0x00;                           // Initialize all GPIO
	P1DIR = 0xFF & ~UART_RXD;               // Set all pins but RXD to output
	DCOCTL = 0;                               // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
	DCOCTL = CALDCO_1MHZ;
	UCA0BR0 = 104;                            // 1MHz 9600
	UCA0BR1 = 0;                              // 1MHz 9600
	UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

	__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled

    __no_operation();

	//return 0;
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){/*
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){*/

    switch(byteNumber){
    case 0:     //calculate and send Length Byte
        while (!(IFG2 & /*UCAYU0TXIFG*/ UCA0TXIFG));                // USCI_A0 TX buffer ready?
        UCA0TXBUF = (UCA0RXBUF - 3);
        IFG2 &= ~UCA0TXIFG; //clear interrupt flag
        break;
    case 1:     //set Red LED PWM
        redPWM(UCA0RXBUF);
        IFG2 &= ~UCA0TXIFG; //clear interrupt flag
        break;
    case 2:     //set Green LED PWm
        greenPWM(UCA0RXBUF);
        IFG2 &= ~UCA0TXIFG; //clear interrupt flag
        break;
    case 3:     //set Blue LED PWM
        bluePWM(UCA0RXBUF);
        IFG2 &= ~UCA0TXIFG; //clear interrupt flag
        break;
    default:    //Send the rest of the data to next node
        while (!(IFG2& /*UCAYU0TXIFG*/ UCA0TXIFG));                // USCI_A0 TX buffer ready?
        UCA0TXBUF = UCA0RXBUF;
        IFG2 &= ~UCA0TXIFG; //clear interrupt flag
        break;
    }

    if(byteNumber < UCA0RXBUF + 3)
        byteNumber++;
    else
        byteNumber = 0;
}

void redPWM(int DC){
    TA0CCR1 = TA0CCR0 * DC;       //assuming 60% Duty cycle is sent as .60
    TA0CTL = TASSEL_2 + MC_1 + ID_0 + TAIE; //TASSEL_2 selects SMCLK as the clock source, and MC_1 tells it to count up to the value in TA0CCR0.
    TA0CCTL1 = OUTMOD_7;    //set-reset mode: see family user guide pg 357
}

void greenPWM(DC){
    TA1CCR1 = TA1CCR0 * DC;       //assuming 60% Duty cycle is sent as .60
    TA1CTL = TASSEL_2 + MC_1 + ID_0 + TAIE; //TASSEL_2 selects SMCLK as the clock source, and MC_1 tells it to count up to the value in TA0CCR0.
    TA1CCTL1 = OUTMOD_7;    //set-reset mode: see family user guide pg 357
}

void bluePWM(DC){
    TA1CCR2 = TA1CCR0 * DC;       //assuming 60% Duty cycle is sent as .60
    TA1CTL = TASSEL_2 + MC_1 + ID_0 + TAIE; //TASSEL_2 selects SMCLK as the clock source, and MC_1 tells it to count up to the value in TA0CCR0.
    TA1CCTL2 = OUTMOD_7;    //set-reset mode: see family user guide pg 357
}




/*  Example code for echoing

// Echo back RXed character, confirm TX buffer is ready first
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  while (!(IFG2&UCAYU0TXIFG));                // USCI_A0 TX buffer ready?
  UCA0TXBUF = UCA0RXBUF;                    // TX -> RXed character
}

*/




