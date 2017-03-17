/*
 * Quick test to read the ds18b20 temperature sensor in a loop and output the
 * reading via the serila uart.
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
#include "ow_gpio.h"
#include "ds18b20.h"

int main(void)
{   
	int8_t retval;
	int64_t id;
	struct ow_id *id_breakdown;
	int8_t i;
	double temp;
	int8_t mode;

	uart_init(UART_ECHO_OFF);

	stdout = &uart_stream;
	stdin  = &uart_stream;

	mode = OW_SEARCH_START;

	while (1) {

		ow_init();

		retval = ow_rom_search(&id, mode);
		if (retval != 0) {
			mode = OW_SEARCH_START;
			continue;
		}

		mode = OW_SEARCH_NEXT;

		id_breakdown = (struct ow_id *)&id;

		printf("Family:0x%.2x\r\n", id_breakdown->family);
		printf("Serial:0x");
		for (i = 5; i > -1; i--) {
			printf("%.2x", id_breakdown->serial[i]);
		}
		printf("\r\n");
		printf("CRC:0x%.2x\r\n", id_breakdown->crc);

		printf("Check: ");
		retval = ow_valid_crc(&id);
		if (retval == 0) {
			printf("OK\r\n");
		} else {
			printf("Invalid\r\n");
			continue;

		}

		retval = ds18b20_gettemp(id, &temp);
		if (retval != 0) {
			printf("Temperature sampling failure\r\n");
			continue;
		}

		printf("Temp: %.1f deg C\r\n", temp);

		_delay_ms(2000);

		printf("\r\n");
	}

	return 0;
}

