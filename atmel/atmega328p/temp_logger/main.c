/*
 * Firmware to integrate a dht22 temperature and humidity sensor with a
 * atmega328p (on an arduino nano) and an esp8266 to collect data for 
 * use with emoncms.
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
#include "esp8266.h"
#include "dht22.h"

#ifndef WIFI_SSID
#define WIFI_SSID "Example"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "12345678"
#endif

#define APIKEY "de923ba32aeb481b72619a0fc711a9d9"
#define SERVERIP "192.168.1.88"
#define NODENAME "test"
#define URL_SIZE 256


int main(void) {   
	int8_t retval;
	double temp;
	double humidity;
	char url[URL_SIZE];
	int8_t loop;

	uart_init(UART_ECHO_OFF);

	stdout = &uart_stream;
	stdin  = &uart_stream;

	retval = dht22_init();
	if (retval != 0)
		printf("DHT22 Init error\n");

	while (1) {
		retval = esp8266_init();
		if (retval != 0)
			/*
			printf("ESP8266 Init error\n");
			*/
			continue;

		retval = esp8266_network(WIFI_SSID, WIFI_PASSWORD);
		if (retval != 0)
			/*
			printf("ESP8266 Network error\n");
			*/
			continue;

		loop = 0;

		while (loop == 0) {
			retval = dht22_getdata(&temp, &humidity);
			if (retval != 0) {
				/*
				printf("Failed to get reading\n");
				*/
				continue;
			}

			snprintf(url, URL_SIZE, "/emoncms/input/post.json?apikey=%s&node=%s&json={temp:%.2f,rh:%.2f}", APIKEY, NODENAME, temp, humidity);

			retval = esp8266_http_get(SERVERIP, url);
			if (retval != 0) {
				/* If we fail redo init */
				loop = 1;
				continue;
			}

			_delay_ms(300000);
		}

	}

	return 0;
}

