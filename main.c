#include <msp430.h> 
#include "uscib0_i2c.h"

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    init_i2c_transmit(0x68);


    __enable_interrupt();

    unsigned char dat = 0x01;
    while(1)
    {
        i2c_transmit(1,&dat,0);
    }

	return 0;
}
