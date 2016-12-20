#include <msp430.h> 
#include "uscib0_i2c.h"

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	              // Stop watchdog timer
    P1SEL |= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
    P1SEL2|= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
	

    unsigned char dat   =   0x0F;
//    unsigned char dat2  =   0x00;

    init_i2c_transmit(0x38);
    i2c_transmit(1,&dat,0);

    init_i2c_receive(0x38);
    i2c_receive(1,&dat);

/*
    init_i2c_transmit(0x38);
    i2c_transmit(1,&dat2,0);


    i2c_transmit(1,&dat,0);
*/
    while(1)
    {
        __no_operation();


        __no_operation();
//
    }

//	return 0;
}
