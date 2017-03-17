/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 */
/*
 * ----------------------------------------------------------------------------
 * Updated to handle larger devices having 16-bit addresses
 *                                                 (2007-09-05) Ruwan Jayanetti
 * ----------------------------------------------------------------------------
 */
/*
 * ----------------------------------------------------------------------------
 * i2c pread and pwrite functions added - Martyn Welch (2014-12-10)
 * ----------------------------------------------------------------------------
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <util/twi.h>
#include "i2c.h"

/*
 * Compatibility defines.  This should work on ATmega8, ATmega16,
 * ATmega163, ATmega323 and ATmega128 (IOW: on all devices that
 * provide a builtin TWI interface).
 *
 * On the 128, it defaults to USART 1.
 */
#ifndef UCSRB
# ifdef UCSR1A		/* ATmega128 */
#  define UCSRA UCSR1A
#  define UCSRB UCSR1B
#  define UBRR UBRR1L
#  define UDR UDR1
# else /* ATmega8 */
#  define UCSRA USR
#  define UCSRB UCR
# endif
#endif
#ifndef UBRR
#  define UBRR UBRRL
#endif


uint8_t twi_get_status(void)
{
	twi_stat = TW_STATUS;
	return twi_stat;
}

int8_t twi_start(void)
{
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	while ((TWCR & _BV(TWINT)) == 0) ;

	switch (twi_get_status()) {
		case TW_REP_START:
		case TW_START:
			break;

		case TW_MT_ARB_LOST:
			i2c_errno = I2C_AGAIN;
			return -1;

		default:
			i2c_errno = I2C_IO;
			return -1;
	}

	return 0;
}

void twi_stop(void)
{
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

uint8_t twi_write(int8_t data)
{
	TWDR = data;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while ((TWCR & _BV(TWINT)) == 0);

	return 0;
}

uint8_t twi_read_ack(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
	while ((TWCR & _BV(TWINT)) == 0);
	return TWDR;
}

uint8_t twi_read_nack(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN);
	while ((TWCR & _BV(TWINT)) == 0);
	return TWDR;
}


void i2c_init(char speed)
{
	switch(speed) {
		case I2C_FREQ_100:
			#if defined(TWPS0)
			/* has prescaler (mega128 & newer) */
			TWSR = 0;
			#endif

			#if F_CPU < 3600000UL
			/* smallest TWBR value, see note [5] */
			TWBR = 10;
			#else
			TWBR = (F_CPU / 100000UL - 16) / 2;
			#endif

			break;

		case I2C_FREQ_400:
			#if defined(TWPS0)
			/* has prescaler (mega128 & newer) */
			TWSR = 0;
			#endif

			TWBR = 0x0C;

			break;

	}

	/* Enable TWI */
	TWCR = _BV(TWEN);
}


int8_t i2c_read(uint8_t addr, uint8_t *buf, int8_t len)
{
	int8_t retval;
	uint8_t data;

	retval = twi_start();
	if (retval < 0) {
		goto err_start;
	}

	twi_write((addr << 1) | TW_READ);
	switch (twi_get_status()) {
		case TW_MR_SLA_ACK:
			break;

		case TW_MR_SLA_NACK:
			i2c_errno = I2C_BUSY;
			retval = -1;
			goto error;

		case TW_MR_ARB_LOST:
			i2c_errno = I2C_AGAIN;
			retval = -1;
			goto error;

		default:
			/* i2c_errno = I2C_IO; */
			i2c_errno = 1;
			retval = -1;
			goto error;
	}

	while (len > 1) {
		data = twi_read_ack();
		switch (twi_get_status()) {
			case TW_MR_DATA_NACK:
				*buf++ = data;
				retval++;
				goto error;

			case TW_MR_DATA_ACK:
				*buf++ = data;
				retval++;
				break;

			default:
				/* i2c_errno = I2C_IO; */
				i2c_errno = 2;
				retval = -1;
				goto error;
		}

		len--;
	}

	/* Send NAK on last byte */
	data = twi_read_nack();
	switch (twi_get_status()) {
		case TW_MR_DATA_NACK:
			*buf++ = data;
			retval++;
			goto error;

		case TW_MR_DATA_ACK:
			*buf++ = data;
			retval++;
			break;

		default:
			/* i2c_errno = I2C_IO; */
			i2c_errno = 3;
			retval = -1;
	}


error:
	twi_stop();
err_start:
	return retval;
	
}


int8_t i2c_write(uint8_t addr, uint8_t *buf, int8_t len)
{
	int8_t retval;

	retval = twi_start();
	if (retval < 0) {
		goto err_start;
	}

	twi_write((addr << 1) | TW_WRITE);
	switch (twi_get_status()) {
		case TW_MT_SLA_ACK:
			break;

		case TW_MT_SLA_NACK:
			i2c_errno = I2C_BUSY;
			retval = -1;
			goto error;

		case TW_MT_ARB_LOST:
			i2c_errno = I2C_AGAIN;
			retval = -1;
			goto error;

		default:
			i2c_errno = I2C_IO;
			retval = -1;
			goto error;
	}

	while (len > 0) {
		twi_write(*buf++);
		switch (twi_get_status()) {
			case TW_MT_DATA_ACK:
				retval++;
				break;

			case TW_MT_DATA_NACK:
			default:
				i2c_errno = I2C_IO;
				retval = -1;
				goto error;
		}

		len--;
	}

error:
	twi_stop();
err_start:
	return retval;

}


int8_t i2c_pread(uint8_t addr, uint8_t *buf, int8_t count, uint8_t offset) 
{ 
        int8_t retval; 
 
        retval = i2c_write(addr, &offset, 1); 
        if (retval != 1) 
                return -1; 
 
        return i2c_read(addr, buf, count); 
} 


int8_t i2c_pwrite(uint8_t addr, uint8_t *buf, int8_t count, uint8_t offset) 
{ 
	int8_t retval;

	retval = twi_start();
	if (retval < 0) {
		goto err_start;
	}

	twi_write((addr << 1) | TW_WRITE);
	switch (twi_get_status()) {
		case TW_MT_SLA_ACK:
			break;

		case TW_MT_SLA_NACK:
			i2c_errno = I2C_BUSY;
			retval = -1;
			goto error;

		case TW_MT_ARB_LOST:
			i2c_errno = I2C_AGAIN;
			retval = -1;
			goto error;

		default:
			i2c_errno = I2C_IO;
			retval = -1;
			goto error;
	}

	twi_write(offset);
	switch (twi_get_status()) {
		case TW_MT_DATA_ACK:
			break;

		case TW_MT_DATA_NACK:
		default:
			i2c_errno = I2C_IO;
			retval = -1;
			goto error;
	}

	while (count > 0) {
		twi_write(*buf++);
		switch (twi_get_status()) {
			case TW_MT_DATA_ACK:
				retval++;
				break;

			case TW_MT_DATA_NACK:
			default:
				i2c_errno = I2C_IO;
				retval = -1;
				goto error;
		}

		count--;
	}

error:
	twi_stop();
err_start:
	return retval;
} 


void i2c_error(void)
{
	printf("ERROR: I2C Error %d\n", i2c_errno);
}

