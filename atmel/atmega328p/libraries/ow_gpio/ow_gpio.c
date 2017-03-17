/*
 * Library to drive 1 wire devices via GPIO.
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

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "ow_gpio.h"

static int8_t ow_search_last_branch;
static int64_t ow_search_last_rom;


void ow_init(void)
{
	/* Set pin to high */
	OW_GPIO_PORT |= _BV(OW_GPIO_BIT);

	/* Set data pin as output */
	OW_GPIO_DDR |= _BV(OW_GPIO_BIT);

	return;
}


int8_t ow_reset(void)
{
	int8_t state;

	/* Set pin to low */
	OW_GPIO_PORT &= ~_BV(OW_GPIO_BIT);

	/* Wait atleast 480uS */
	_delay_us(480);

	/* Set pin to high */
	OW_GPIO_PORT |= _BV(OW_GPIO_BIT);

	/* Set data pin as input */
	OW_GPIO_DDR &= ~_BV(OW_GPIO_BIT);

	/* Wait for 45uS */
	_delay_us(100);

	/* Slave should be pulling line low */
	state = OW_GPIO_PIN & _BV(OW_GPIO_BIT);

	/* Return error if no presence pulse detected */
	if (state != 0) {
		/* Set data pin as output */
		OW_GPIO_DDR |= _BV(OW_GPIO_BIT);

		return -1;
	}

	/* Allow devices to recover (important if using parasitic power) */
	_delay_us(380);

	/* Set data pin as output */
	OW_GPIO_DDR |= _BV(OW_GPIO_BIT);

	return 0;
}


int8_t ow_read_bit(void)
{
	int8_t state;

	/* Set data pin as output */
	OW_GPIO_DDR |= _BV(OW_GPIO_BIT);

	/* Set pin to low */
	OW_GPIO_PORT &= ~_BV(OW_GPIO_BIT);

	/* Low delay */
	_delay_us(1);

	/* Set pin to high */
	OW_GPIO_PORT |= _BV(OW_GPIO_BIT);

	/* Set data pin as input */
	OW_GPIO_DDR &= ~_BV(OW_GPIO_BIT);

	/* Read delay */
	_delay_us(4);

	/* Read state */
	state = OW_GPIO_PIN & _BV(OW_GPIO_BIT); 

	/* Set data pin as output */
	OW_GPIO_DDR |= _BV(OW_GPIO_BIT);

	_delay_us(40);

	if (state == 0) {
		return 0;
	} else {
		return 1;
	}
}


int8_t ow_read(int8_t *data, int8_t count)
{
	int8_t i;
	int8_t j;
	int8_t bit;

	for (j = 0; j < count; j++) {
		*data = 0;

		for (i = 0; i < 8; i++) {
			/* Read bit and it's complement */
			bit = ow_read_bit();

			/* Shift values down */
			*data = (*data >> 1);

			/*
			 * Set or clear top bit. Clear necessary due to the effect of shifting a
			 * register with a set top bit.
			 */
			if (bit) {
				*data |= 0x80;
			} else {
				*data &= ~0x80;
			}
		}

		data++;
	}

	return 0;
}


int8_t ow_write_bit(int8_t value)
{
	/* Set data pin as output */
	OW_GPIO_DDR |= _BV(OW_GPIO_BIT);

	/* Set pin to low */
	OW_GPIO_PORT &= ~_BV(OW_GPIO_BIT);

	/* Low delay */
	if (value == 0) {
		_delay_us(60);
	} else {
		_delay_us(5);
	}

	/* Set pin to high */
	OW_GPIO_PORT |= _BV(OW_GPIO_BIT);

	/* High delay */
	if (value == 0) {
		_delay_us(40);
	} else {
		_delay_us(95);
	}

	return 0;
}


int8_t ow_write(int8_t *data, int8_t count)
{
	int8_t i;
	int8_t j;

	for (j = 0; j < count; j++) {

		for (i = 0; i < 8; i++) {
			if ((*data & _BV(i)) == 0) {
				ow_write_bit(0);
			} else {
				ow_write_bit(1);
			}
		}

		data++;
	}

	return 0;
}

int8_t ow_valid_crc(int64_t *id)
{
	int8_t i;
	uint8_t crc = 0;

	uint8_t ow_crc_table[] = {
		0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
		157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
		35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
		190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
		70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
		219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
		101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
		248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
		140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
		17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
		175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
		50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
		202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
		87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
		233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
		116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

	for (i = 0; i < 64; i++) {
		crc = ow_crc_table[crc ^ ((*id >> i) & 0x1)];
	}

	if (crc == 0) {
		return 0;
	} else {
		return 1;
	}
}


int8_t ow_rom_search(int64_t *id, int8_t state)
{
	int8_t retval;
	int8_t i;
	int8_t data;
	int8_t bit;
	int8_t complement;
	int8_t follow;
	int8_t last_branch;

	if (state == OW_SEARCH_START) {
		ow_search_last_branch = -1;
		ow_search_last_rom = 0;
	} else {
		if (ow_search_last_branch == -1) {
			return -1;
		} 
	}

	last_branch = -1;

	retval = ow_reset();
	if (retval != 0)
		return -1;

	/* Write search command */
	data = OW_CMD_SEARCH;
	ow_write(&data, 1);

	*id = 0;
	for (i = 0; i < 64; i++) {
		/* Read bit and it's complement */
		bit = ow_read_bit();
		complement = ow_read_bit();

		if (bit != complement) {
			follow = bit;
		} else {
			if (bit == 1) {
				return -1;
			}

			if (ow_search_last_branch == i) {
				follow = 1;
			} else if (i > ow_search_last_branch) {
				follow = 0;
			} else {
				follow = (int8_t)((ow_search_last_rom >> i) & 0x1);
			}

			if (follow == 0) {
				last_branch = i;
			}
		}

		/* Write value back */
		ow_write_bit(follow);

		/* Shift values down */
		*id = (*id >> 1);

		/*
		 * Set or clear top bit. Clear necessary due to the effect of
		 * shifting a register with a set top bit.
		 */
		if (follow) {
			*id |= 0x8000000000000000LL;
		} else {
			*id &= ~0x8000000000000000LL;
		}
	}

	ow_search_last_rom = *id;
	ow_search_last_branch = last_branch;
	return 0;
}


int8_t ow_rom_match(int64_t *id)
{
	int8_t retval;
	int8_t data;

	retval = ow_reset();
	if (retval != 0)
		return -1;

	data = OW_CMD_ROM_MATCH;
	ow_write(&data, 1);

	retval = ow_write((int8_t *)id, 8);

	return 0;
}


int8_t ow_rom_read(int64_t *id)
{
	int8_t retval;
	int8_t data;

	retval = ow_reset();
	if (retval != 0)
		return -1;

	data = OW_CMD_ROM_READ;
	ow_write(&data, 1);

	retval = ow_read((int8_t *)id, 8);

	return 0;
}


int8_t ow_scratchpad_read(int8_t *data, int8_t count)
{
	int8_t cmd;

	cmd = 0xBE;
	ow_write(&cmd, 1);

	return ow_read(data, 8);
}

