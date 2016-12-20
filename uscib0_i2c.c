#include <msp430.h>
#include "uscib0_i2c.h"
/*
 * uscib0_i2c.c
 *
 *  Created on: 2016. dec. 15.
 *      Author: lmajer
 */

unsigned char test;

unsigned char rxbytecounter, txbytecounter;         // RX/TX byte counter
unsigned char *rxptr;                               // RX pointer
unsigned char *txptr;                               // TX pointer
unsigned char RPT_flag  =   0;                      // Repeat flag
unsigned char RX_flag   =   0;

void init_i2c_transmit(unsigned char address)
{
    __disable_interrupt();                          // Disable interrupt
    RX_flag =   0;                                  // Transmit
    IE2 &= ~UCB0RXIE;                               // Disable RX interrupt
    while (UCB0CTL1 & UCTXSTP);                     // Ensure stop condition got sent// Disable RX interrupt
    UCB0CTL1 |= UCSWRST;                            // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;           // I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;                  // Use SMCLK, keep SW reset
    UCB0BR0 = 12;                                   // fSCL = SMCLK/12 = ~100kHz
    UCB0BR1 = 0;
    UCB0I2CSA = address;                            // Slave Address is input parameter
    UCB0CTL1 &= ~UCSWRST;                           // Clear SW reset, resume operation
    IE2 |= UCB0TXIE;                                // Enable TX interrupt
}

void init_i2c_receive(unsigned char address)
{
    __disable_interrupt();
    RX_flag = 1;
    IE2 &= ~UCB0TXIE;
    UCB0CTL1 |= UCSWRST;                            // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;           // I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;                  // Use SMCLK, keep SW reset
    UCB0BR0 = 12;                                   // fSCL = SMCLK/12 = ~100kHz
    UCB0BR1 = 0;
    UCB0I2CSA = address;                               // Slave Address is input parameter
    UCB0CTL1 &= ~UCSWRST;                           // Clear SW reset, resume operation
    IE2 |= UCB0RXIE;                                // Enable RX interrupt
}

void i2c_transmit(unsigned char txcnt, unsigned char *txbytes, unsigned char repeat)
{
    RPT_flag    =   repeat;                         // Load repeat flag
    txptr   =   txbytes;                            // Load TX pointer
    txbytecounter   =   txcnt;                      // Load TX byte counter
    while (UCB0CTL1 & UCTXSTP);                     // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;                     // I2C TX, start condition
    __bis_SR_register(CPUOFF + GIE);                // Enter LPM0 w/ interrupts
}

void i2c_receive(unsigned char rxcnt, unsigned char *rxbytes)
{
    rxbytecounter   =   rxcnt;
    rxptr   =   rxbytes;

    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTXSTT;                    // I2C start condition
    if(rxbytecounter == 1)
    {
        while (UCB0CTL1 & UCTXSTT);         // Start condition sent?
        UCB0CTL1 |= UCTXSTP;                // Generate I2C stop condition
    }
    __bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts
}




#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCIAB0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if(RX_flag)                                         // Master Receiver mode?
    {
        rxbytecounter--;                                // Decrement rx counter
        if(rxbytecounter)                               // Rx counter 0?
        {
            *rxptr    =   UCB0RXBUF;                    // Read RX Buffer
            rxptr++;                                    // Increment Pointer
            if(rxbytecounter    ==  1)                  // 1 character left?
                UCB0CTL1    |=  UCTXSTP;                // Generate i2c Stop
        }
        else                                            // RX Counter = 0
        {
            *rxptr    =   UCB0RXBUF;                    // Read the last character
            __bic_SR_register_on_exit(CPUOFF);          // Exit LMP0 (wake up CPU) Disable interrupts
        }
    }
    else                                                // Master Transmitter Mode
    {
        if(txbytecounter)
        {
            UCB0TXBUF   =   *txptr;              // Transmit pointer
            txptr++;                                    // Increment TX Pointer
            txbytecounter--;                            // Decrement TX Byte counter
        }
        else
        {
            UCB0CTL1    |=  UCTXSTP;                // Generate i2c Stop
            IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
            __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
        }
    }
}

