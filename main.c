#include <msp430.h> 
#include "uscib0_i2c.h"

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    P1SEL |= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
    P1SEL2|= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
	
    init_i2c_transmit(0x68);


    __enable_interrupt();

    unsigned char dat = 0x01;
    while(1)
    {
        i2c_transmit(1,&dat,0);
    }

	return 0;
}
