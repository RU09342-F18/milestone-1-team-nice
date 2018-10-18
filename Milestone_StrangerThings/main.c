#include <msp430g2553.h>

//Milestone 1
/*
 * Written by Scott Wood and David Sheppard
 * October 2018
 * Controls an RGB LED using Software PWM and timers
 * Data is set through UART which specifies the values for the colors
 */

#define UART_RXD BIT0
/*
void redPWM(int DC);
void greenPWM(int DC);
void bluePWM(int DC);*/

int byteNumber = 0;
int numberOfBytes = 0;

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    BCSCTL3 = LFXT1S_2;                     //interfaces with crystal (needed for clock to work)

    P2DIR |= BIT1 + BIT3 + BIT5;
    P2SEL2 &= ~(BIT1 + BIT3 + BIT5);
    P2SEL &= ~(BIT1 + BIT3 + BIT5);
    P1SEL |= BIT1 + BIT2;   //lets device receive uart data - see datasheet pg 42-43
    P1SEL2 |= BIT1 + BIT2;  //lets device receive uart data

    TA1CCR1 = 0x7FFF;
    TA1CCR2 = 0x7FFF;
    TA1CCR0 = 0x7FFF;

    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_8MHZ;                    // Set clock to 8 MHz
    DCOCTL = CALDCO_8MHZ;
    UCA0BR0 = 0x41;                            // 8MHz with 9600 BAUD rate
    UCA0BR1 = 0x03;                              // 8MHz 9600

    UCA0MCTL = UCBRS_3 + UCBRF_0;             // Modulation UCBRSx = 1
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

    UCA0TXBUF = 0;

    //TASSEL_2 selects SMCLK as the clock source, and MC_2 is continuous mode. No divider, enable interrupts, clear everything.
    TA1CTL = TASSEL_2 + MC_2 + ID_0 + TAIE + TACLR;

    //enable interrupts for CCR0, 1, and 2 of TA1
    TA1CCTL1 = CCIE;
    TA1CCTL0 = CCIE;
    TA1CCTL2 = CCIE;

    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled


}

#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A(void){

    int x = TA1IV;
    switch(x){
    case 2: //red (CCR1) overflow
        P2OUT |= BIT1;
        break;
    case 4: //blue (CCR2) overflow
        P2OUT |= BIT5;
        break;
    case 10:    //timer overflow
        P2OUT &= ~(BIT1 + BIT3 + BIT5);
        break;
    }
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMERA1_CCR0(void){

    P2OUT |= BIT3;  //green (CCR0) overflow
}


#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){

    /*
     * note: if 0x00 is sent through UART, the light would turn on fully because the CCR interrupt when
        CCR = 0 cannot be generated fast enough after complete timer overflow.
        Therefore, when the RX input is 0x00, we set the CCR to 20, which is small enough to appear off, but
        is large enough that the processor will have time to overflow when it is reached.
     */

    switch(byteNumber){
    case 0:     //calculate and send Length Byte
        numberOfBytes = UCA0RXBUF;
        break;
    case 1:     //set Red LED PWM
        TA1CCR1 = ((UCA0RXBUF*256));  //0xFFFF / 257 = 255
        if(UCA0RXBUF == 0x00)
            TA1CCR1 = 30;
        break;
    case 2:     //set Green LED PWm
        TA1CCR0 = ((UCA0RXBUF*256));  //0xFFFF / 257 = 255
        if(UCA0RXBUF == 0x00)
            TA1CCR0 = 20;
        break;
    case 3:     //set Blue LED PWM
        TA1CCR2 = ((UCA0RXBUF*255));  //0xFFFF / 257 = 255
        if(UCA0RXBUF == 0x00)
            TA1CCR2 = 20;

        //send off size byte now
        while (!(IFG2 & UCA0TXIFG));                // USCI_A0 TX buffer ready?
        UCA0TXBUF = (numberOfBytes - 3);
        break;
    default:    //Send the rest of the data to next node
        while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
        UCA0TXBUF = UCA0RXBUF;
        break;
    }

    //increment byte number
    if(byteNumber < numberOfBytes)
        byteNumber++;
    else    //reset byte number when all bytes have been sent
        byteNumber = 0;
}






