/*
 * Library to drive dht22 temperature and humidity sensor.
 */

/*
 * Copyright 2015 Martyn Welch <martyn@welchs.me.uk>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 *  * Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#include "dht22.h"

int8_t dht22_init(void)
{

	/* Set data pin as output */
	DHT22_DDR |= _BV(DHT22_BIT);

	/* Set pin to high */
	DHT22_PORT |= _BV(DHT22_BIT);

	return 0;
}


int8_t dht22_getdata(double *temp, double *humidity)
{
	int8_t i;
	int8_t j;
	uint8_t data[5];
	uint8_t chk_calc;
	uint16_t bitlow;
	uint16_t bithigh;

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	/* Ensure data pin set to output */
	DHT22_DDR |= _BV(DHT22_BIT);

	/* Hold data pin low for at least 18ms */
	DHT22_PORT &= ~_BV(DHT22_BIT);

	_delay_ms(18);

	/* Pull back up */
	DHT22_PORT |= _BV(DHT22_BIT);

	/* Switch to input */
	DHT22_DDR &= ~_BV(DHT22_BIT);


	/* Pre-amble */

	/* Wait for low */
	while ((DHT22_PIN & _BV(DHT22_BIT)) != 0);

	/* Wait for high */
	while ((DHT22_PIN & _BV(DHT22_BIT)) != _BV(DHT22_BIT));

	/* Wait for low */
	while ((DHT22_PIN & _BV(DHT22_BIT)) != 0);

	/* Data */
	for (j = 0; j < 5; j++) {
		for (i = 0; i < 8; i++) {
			bitlow = 0;
			bithigh = 0;

			/* Wait for high */
			while ((DHT22_PIN & _BV(DHT22_BIT)) != _BV(DHT22_BIT)) {
				bitlow++;
			}

			/* Wait for low */
			while ((DHT22_PIN & _BV(DHT22_BIT)) != 0) {
				bithigh++;
			}

			if (bithigh > bitlow) {
				data[j] |= _BV(7 - i);
			}
		}
	}

	chk_calc = data[0] + data[1] + data[2] + data[3];

	if (chk_calc != data[4]) {
		return 1;
	}

	*temp = (((data[2] & 0x7F) * 256) + data[3]) / 10;
	if (data[2] & 0x80)
		*temp = *temp * -1;

	*humidity = ((data[0] * 256) + data[1]) / 10;

	return 0;
}

