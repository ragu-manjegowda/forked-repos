/*
 * Interactive tool, interfacing a tcn75a and m41t81s and allowing them to be
 * configured / read via commands on the serial port.
 */

/*
 * BUGS:
 *
 * - "log" function enables the interupt of the m41t81s with the intention of
 *   reading the tcn75a when it fires. This is failing resulting in an invalid
 *   temperature reading being output (0 deg C).
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
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#include "uart.h"
#include "i2c.h"
#include "m41t81s.h"
#include "tcn75a.h"
#include "readline.h"

#define PROMPT "> "
#define INPUT_BUF_SIZE 1024

uint8_t app_retval;

int8_t cmd_date(char *params)
{
	int8_t retval;
	int16_t year;
	int8_t month, day;
	char *sep;
	char *ret;
	char strnum[5];

	retval = strlen(params);

	if (retval == 0) {
		retval = m41t81s_getdate(&year, &month, &day);
		if (retval == -1) {
			i2c_error();
			return -1;
		}

		printf("%04d-%02d-%02d\r\n", year, month, day);

		return 0;
	}

#if 0
	if (strcmp("-h", params) == 0){
		printf("date: Display or set date\r\n\r\n");
		printf("No parameters displays date\r\n");
		printf("-h:\tDisplay help\r\n");
		printf("-s <date>:\tSet date. Requires further parameter with date as yyyy-mm-dd\r\n");
	}
#endif

	if (strncmp("-s", params, 2) == 0){
		sep = strchr(params, ' ');
		if (sep == NULL) {
			printf("Can't parse date\r\n");
			return -1;
		}

		ret = strncpy(strnum, sep + 1, 4);
		if (ret == NULL) {
			printf("Failed to copy year\r\n");
			return -1;
		}

		strnum[4] = '\0';
		year = atoi(strnum);

		sep = strchr(sep, '-');
		if (sep == NULL) {
			printf("Can't parse date\r\n");
			return -1;
		}

		ret = strncpy(strnum, sep + 1, 2);
		if (ret == NULL) {
			printf("Failed to copy month\r\n");
			return -1;
		}

		strnum[2] = '\0';
		month = atoi(strnum);

		sep = strchr(sep + 1, '-');
		if (sep == NULL) {
			printf("Can't parse date\r\n");
			return -1;
		}

		ret = strncpy(strnum, sep + 1, 2);
		if (ret == NULL) {
			printf("Failed to copy day\r\n");
			return -1;
		}

		strnum[2] = '\0';
		day = atoi(strnum);

		printf("%04d-%02d-%02d\r\n", year, month, day);

		retval = m41t81s_setdate(year, month, day);
		if (retval != 0) {
			printf("Failed to set date\r\n");
			return -1;
		}
	}

	return 0;
}

int8_t cmd_echo(char *params)
{
	if (strncmp(params, "$?", 2 ) == 0) {
		printf("%d\r\n", app_retval);
		return 0;
	}

	printf("%s\r\n", params);

	return 0;
}

int8_t cmd_log(char *params)
{
	int8_t retval;

	retval = m41t81s_setalarm(1, 1, 0, 0, 0, M41T81S_ALARM_PER_MIN | M41T81S_ALARM_INT);
	if (retval != 0)
		printf("Failed to set alarm\r\n");

	/* Set PD2(INT0) as input */ 
	DDRD = 1<<PD2;
	/* Enable PD2 pull-up resistor */
	PORTD = 1<<PD2;
 
	/* Enable INT0 */
	EIMSK = 1<<INT0;
	/* Trigger INT0 on falling edge */
	EICRA = 1<<ISC01;
 
	/* Enable Global Interrupt */
	sei();

	return 0;
}

/* Interrupt routine for logging */
ISR(INT0_vect)
{
	int8_t retval;
	int16_t year;
	int8_t month, day;
	int8_t hour, minute, second;
	uint8_t int_data;
	int32_t temp;
	char strtemp[7];

	retval = i2c_pread(0x68, &int_data, 1, 0x0F);
	if (retval != 1) {
		printf("INT: Reading RTC failed\r\n");
		return;
	}

	retval = m41t81s_gettime(&hour, &minute, &second);
	if (retval == -1){
		i2c_error();
		return;
	}

	retval = m41t81s_getdate(&year, &month, &day);
	if (retval == -1) {
		i2c_error();
		return;
	}

	retval = tcn75a_gettemp(&temp);
	if (retval == -1) {
		printf("INT: Failed to retrieve temperature\r\n");
		return;
	}

#if 0
	sprintf(strtemp, "%0.2f", temp);
	printf("%04d-%02d-%02d %02d:%02d:%02d %s\r\n", year, month, day, hour, minute, second, strtemp);
#else
	printf("%04d-%02d-%02d ", year, month, day);
	printf("%02d:%02d:%02d ", hour, minute, second);
	printf("%d\r\n", temp);
#endif
}

int8_t cmd_temp(char *params)
{
	int8_t retval;
	int32_t temp;

	retval = tcn75a_gettemp(&temp);
	if (retval == -1) {
		printf("Failed to retrieve temperature\r\n");
		return -1;
	}

	printf("%d\r\n", temp);

	return 0;
}

int8_t cmd_time(char *params)
{
	int8_t retval;
	int8_t hour, minute, second;
	char *sep;
	char *ret;
	char strnum[3];

	retval = strlen(params);

	if (retval == 0) {
		retval = m41t81s_gettime(&hour, &minute, &second);
		if (retval == -1){
			i2c_error();
			return -1;
		}

		printf("%02d:%02d:%02d\r\n", hour, minute, second);

		return 0;
	}
#if 0
	if (strcmp("-h", params) == 0){
		printf("time: Display or set time\r\n\r\n");
		printf("No parameters displays time\r\n");
		printf("-h:\tDisplay help\r\n");
		printf("-s <time>:\tSet time. Requires further parameter with time as hh:mm:ss\r\n");
	}
#endif
	if (strncmp("-s", params, 2) == 0){
		sep = strchr(params, ' ');
		if (sep == NULL) {
			printf("Can't parse time\r\n");
			return -1;
		}

		ret = strncpy(strnum, sep + 1, 2);
		if (ret == NULL) {
			printf("Failed to copy hours\r\n");
			return -1;
		}

		strnum[2] = '\0';
		hour = atoi(strnum);

		sep = strchr(sep, ':');
		if (sep == NULL) {
			printf("Can't parse time\r\n");
			return -1;
		}

		ret = strncpy(strnum, sep + 1, 2);
		if (ret == NULL) {
			printf("Failed to copy minutes\r\n");
			return -1;
		}

		strnum[2] = '\0';
		minute = atoi(strnum);

		sep = strchr(sep + 1, ':');
		if (sep == NULL) {
			printf("Can't parse time\r\n");
			return -1;
		}

		ret = strncpy(strnum, sep + 1, 2);
		if (ret == NULL) {
			printf("Failed to copy seconds\r\n");
			return -1;
		}

		strnum[2] = '\0';
		second = atoi(strnum);

		printf("%02d:%02d:%02d\r\n", hour, minute, second);

		retval = m41t81s_settime(hour, minute, second);
		if (retval != 0) {
			printf("Failed to set time\r\n");
			return -1;
		}
	}

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

	space = strchr(input, ' ');
	params = space + 1;
	command = input;
	*space = '\0';

	if (strcmp(command, "date") == 0)
		return cmd_date(params);

	if (strcmp(command, "echo") == 0)
		return cmd_echo(params);

	if (strcmp(command, "log") == 0)
		return cmd_log(params);

	if (strcmp(command, "temp") == 0)
		return cmd_temp(params);

	if (strcmp(command, "time") == 0)
		return cmd_time(params);

	printf("Command not found\r\n");
	return -1;
}

int main(void) {   
	char input[INPUT_BUF_SIZE];
	int8_t retval;

	uart_init(UART_ECHO_ON);

	i2c_init(I2C_FREQ_100);

	stdout = &uart_stream;
	stdin  = &uart_stream;

	retval = m41t81s_init();
	if (retval != 0)
		printf("RTC Init error\r\n");

	retval = tcn75a_init();
	if (retval != 0)
		printf("Temp Init error");

	while (1) {
		printf("%s", PROMPT);

		readline(input, INPUT_BUF_SIZE);
		printf("\r\n");

		app_retval = process_commands(input);
	}

	return 0;
}

