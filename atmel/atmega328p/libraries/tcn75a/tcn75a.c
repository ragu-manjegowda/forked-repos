/*
 * Library to drive tcn75a temperature sensor.
 *
 * Requires i2c library.
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

#include "i2c.h"
#include "tcn75a.h"


int8_t tcn75a_init(void)
{
	/* We are currently just relying on the power-up defaults */
	
	return 0;
}

#if 0

int8_t tcn75a_gettemp(double *temp)
{
	uint8_t data[2];
	uint8_t i;
	int8_t retval;

	retval = i2c_pread(0x48, data, 2, 0x0);
	if (retval != 2) {
		return -1;
	}

	*temp = 0;

	for (i = 0; i < 8; i++) {
		if (data[0] & (1 << i)) {
			*temp += (1 << i);
		}
	}

        for (i = 7; i > 3; i--) {
                if (data[1] & (1 << i)) {
                        *temp += (1.0/(1<<(8 - i)));
                }
        }

	return 0;
}

#else

int8_t tcn75a_gettemp(int32_t *temp)
{
	uint8_t data[2];
	uint8_t i;
	int8_t retval;

	retval = i2c_pread(0x48, data, 2, 0x0);
	if (retval != 2) {
		return -1;
	}

	*temp = 0;

	for (i = 0; i < 8; i++) {
		if (data[0] & (1 << i)) {
			*temp += (1000 * (1 << i));
		}
	}

        for (i = 7; i > 3; i--) {
                if (data[1] & (1 << i)) {
                        *temp += (1000/(1<<(8 - i)));
                }
        }

	return 0;
}

#endif
