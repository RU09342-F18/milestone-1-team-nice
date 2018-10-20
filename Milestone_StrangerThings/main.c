#include <msp430g2553.h>

//Milestone 1
/*
 * Written by Scott Wood and David Sheppard
 * October 2018
 * Controls an RGB LED using Software PWM and timers
 * Data is set through UART which specifies the values for the colors.
 * Code allows for multiple devices to be strung together, passing data out through TX line to next device.
 */

#define UART_RXD BIT0


int byteNumber = 0;
int numberOfBytes = 0;
int isZero0;    //=0 when red LED is supposed to be off, else =1
int isZero1;    //=0 when green LED is supposed to be off, else =1
int isZero2;    //=0 when blue LED is supposed to be off, else =1

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P2OUT = 0xFF;    //initialize LED of off

    //initialize isZero values
    isZero0 = 0;
    isZero1 = 0;
    isZero2 = 0;

    BCSCTL3 = LFXT1S_2;                     //interfaces with crystal (needed for clock to work)

    P2DIR |= BIT1 + BIT3 + BIT5;            //set RGB pins to be outputs
    P2SEL2 &= ~(BIT1 + BIT3 + BIT5);        //set input source
    P2SEL &= ~(BIT1 + BIT3 + BIT5);         //set input source
    P1SEL |= BIT1 + BIT2;                   //lets device receive uart data - see datasheet pg 42-43
    P1SEL2 |= BIT1 + BIT2;                  //lets device receive uart data

    DCOCTL = 0;                             // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_8MHZ;                  // Set clock to 8 MHz
    DCOCTL = CALDCO_8MHZ;
    UCA0BR0 = 0x41;                         // 8MHz with 9600 BAUD rate
    UCA0BR1 = 0x03;                         // 8MHz 9600

    UCA0MCTL = UCBRS_3 + UCBRF_0;           // Modulation UCBRSx = 1
    UCA0CTL1 |= UCSSEL_2;                   // SMCLK
    UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                        // Enable USCI_A0 RX interrupt

    UCA0TXBUF = 0;                          //set RX buffer to 0 for testing purposes

    //TASSEL_2 selects SMCLK as the clock source, and MC_2 is continuous mode. No divider, enable interrupts, clear everything.
    TA1CTL = TASSEL_2 + MC_2 + ID_0 + TAIE + TACLR;

    //enable interrupts for CCR0, 1, and 2 of TA1
    TA1CCTL1 = CCIE;
    TA1CCTL0 = CCIE;
    TA1CCTL2 = CCIE;

    __bis_SR_register(LPM0_bits + GIE);       // Enter low power mode, interrupts enabled


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
        if(isZero0 != 0)        //only turn on LED if its value is not set to 0
            P2OUT &= ~(BIT1);
        if(isZero1 != 0)        //only turn on LED if its value is not set to 0
            P2OUT &= ~(BIT3);
        if(isZero2 != 0)        //only turn on LED if its value is not set to 0
            P2OUT &= ~(BIT5);
        break;
    }
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMERA1_CCR0(void){

    P2OUT |= BIT3;  //green (CCR0) overflow
}


#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){

    switch(byteNumber){
    case 0:     //calculate and send Length Byte
        numberOfBytes = UCA0RXBUF;
        break;
    case 1:     //set Red LED PWM
        TA1CCR1 = ((UCA0RXBUF*256));  //0xFFFF / 257 = 255
        if(UCA0RXBUF == 0x00){  //if LED should be off, set isZero to 0
            isZero0 = 0;
        }
        else                    //otherwise isZero needs to be 1
            isZero0 = 1;
        break;
    case 2:     //set Green LED PWm
        TA1CCR0 = ((UCA0RXBUF*256));  //0xFFFF / 257 = 255
        if(UCA0RXBUF == 0x00){  //if LED should be off, set isZero to 0
            isZero1 = 0;
        }else                    //otherwise isZero needs to be 1
            isZero1 = 1;
        break;
    case 3:     //set Blue LED PWM
        TA1CCR2 = ((UCA0RXBUF*255));  //0xFFFF / 257 = 255
        if(UCA0RXBUF == 0x00){  //if LED should be off, set isZero to 0
            isZero2 = 0;
        }else                    //otherwise isZero needs to be 1
            isZero2 = 1;

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
    else                //reset byte number when all bytes have been sent (now we an get new data
        byteNumber = 0;
}


