/*
 * Quick example to test m41t81s, including the generation of interupts.
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
#include <stdio.h>
#include <util/delay.h>

#include "uart.h"
#include "i2c.h"
#include "m41t81s.h"


int main(void) {   

	int8_t retval;
	int16_t year;
	int8_t month, day;
	int8_t hour, minute, second;
	int8_t data;
	int8_t dow;

	uart_init(UART_ECHO_OFF);
	i2c_init(I2C_FREQ_100);

	stdout = &uart_stream;
	stdin  = &uart_stream;

	retval = m41t81s_init();
	if (retval != 0)
		printf("Init error\n");

	retval = m41t81s_settime(23,59,45);
	if (retval != 0)
		printf("Failed to set time\n");
		
	retval = m41t81s_setdate(2014,12,30);
	if (retval != 0)
		printf("Failed to set date\n");

	retval = m41t81s_setalarm(1, 1, 0, 0, 0, M41T81S_ALARM_PER_MIN | M41T81S_ALARM_INT);
	if (retval != 0)
		printf("Failed to set alarm\n");

	retval = m41t81s_getdow(&dow);
	if (retval != 0)
		printf("Failed to retrieve day of week\n");
	else
		printf("Day of week: %d\n", dow);
		
	while (1) {
		_delay_ms(1000);

		retval = m41t81s_getdate(&year, &month, &day);
		if (retval == -1)
			i2c_error();

		retval = m41t81s_gettime(&hour, &minute, &second);
		if (retval == -1)
			i2c_error();

		printf("Time: %04d-%02d-%02d %02d:%02d:%02d\n", year, month,
			day, hour, minute, second);

		retval = i2c_pread(0x68, &data, 1, 0x0F);
		if (retval != 1)
			return -1;

		if (data & 0x40)
			printf("Alarm flag set\n");
	}

	return 0;

}
