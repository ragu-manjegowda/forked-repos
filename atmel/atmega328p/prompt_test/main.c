/*
 * Test to create an interactive prompt on the serial uart.
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
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

#include <util/delay.h>

#include "uart.h"
#include "readline.h"

#define PROMPT "> "
#define INPUT_BUF_SIZE 1024

uint8_t app_retval;

int8_t cmd_echo(char *params)
{
	if (strncmp(params, "$?", 2 ) == 0) {
		printf("%d\r\n", app_retval);
		return 0;
	}

	printf("%s\r\n", params);

	return 0;
}

int8_t process_commands(char *input)
{
	int len;
	char *command;
	char *params;
	char *space;

	len = strlen(input);
	if (len == 0)
		return 0;
#if 0
	printf("Len:%d\r\n", len);
#endif
	space = strchr(input, ' ');
	params = space + 1;
	command = input;
	*space = '\0';

#if 0
	printf("Command:%s\r\n", command);
	printf("Params:%s\r\n", params);
#endif

	if (strcmp(command, "echo") == 0)
		return cmd_echo(params);

	printf("Command not found\r\n");
	return -1;
}

int main(void)
{   
	char input[INPUT_BUF_SIZE];

	uart_init(UART_ECHO_ON);

	stdout = &uart_stream;
	stdin  = &uart_stream;

	while (1) {
		printf("%s", PROMPT);

		readline(input, INPUT_BUF_SIZE);
		printf("\r\n");

		app_retval = process_commands(input);
	}

	return 0;

}
