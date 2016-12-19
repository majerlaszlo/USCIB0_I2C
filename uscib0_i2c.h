/*
 * uscib0_i2c.h
 *
 *  Created on: 2016. dec. 15.
 *      Author: lmajer
 */

#ifndef USCIB0_I2C_H_
#define USCIB0_I2C_H_

void init_i2c_transmit(unsigned char address);
void init_i2c_receive(unsigned char address);
void i2c_transmit(unsigned char txcnt, unsigned char *txbytes, unsigned char repeat);
void i2c_receive(unsigned char rxcnt, unsigned char *rxbytes);

#endif /* USCIB0_I2C_H_ */
