/*
 * Library to drive ds18b20 temperature sensor.
 *
 * Requires ow_gpio library.
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
#include "ds18b20.h"


int8_t ds18b20_gettemp(int64_t id, double *temp)
{
	int8_t retval;
	int8_t data[2];
	int8_t bit;
	int16_t value;

	retval = ow_rom_match(&id);
	if (retval != 0) {
		return -1;
	}

	/* Write temp convert command */
	data[0] = 0x44;
	ow_write(data, 1);

	bit = 0;
	while(!bit) {
		_delay_ms(100);
		bit = ow_read_bit();
	}

	retval = ow_rom_match(&id);
	if (retval != 0) {
		return -1;
	}

	retval = ow_scratchpad_read(data, 2);
	if (retval != 0) {
		return -1;
	}

	value = (data[1] << 8) | (data[0] & 0xff);

	*temp = ((double)value) / 16;

	return 0;
}

