/*
 * Library to provide serial access on atmega328p and compatible parts.
 *
 * This library uses buffers to allow the data to be sent from the serial port
 * asynchronously, rather than tying up the microcontroller whilst the data is
 * sent slowly from the uart a character at a time.
 *
 * In addition to using up space as buffers for the serial, this utilises the
 * uart interrupts and will thus also interrupt the flow of execution of the
 * main task when the interrupt triggers. This may or may not be an issue
 * depeding on what else you are attempting to do on the microcontroller.
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
#include <avr/interrupt.h>
#include <stdio.h>

#include "uart.h"

#ifndef BAUD
#define BAUD 9600
#endif
#include <util/setbaud.h>

#ifndef UART_RX_BUFFER_SIZE
#define UART_RX_BUFFER_SIZE 128
#endif

#ifndef UART_TX_BUFFER_SIZE
#define UART_TX_BUFFER_SIZE 128
#endif

struct tx_ring {
    char buffer[UART_TX_BUFFER_SIZE];
    volatile uint8_t start;
    volatile uint8_t end;
};

struct rx_ring {
    char buffer[UART_RX_BUFFER_SIZE];
    volatile uint8_t start;
    volatile uint8_t end;
};

static struct tx_ring tx_buffer;
static struct rx_ring rx_buffer;

FILE uart_stream = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

static int uart_echo;


void uart_init(int settings) {
	/* Use "start = end" for empty buffer. */
	tx_buffer.start = 0;
	tx_buffer.end   = 0;

	/* Use "start = end" for empty buffer. */
	rx_buffer.start = 0;
	rx_buffer.end   = 0;

	if (settings & UART_ECHO_ON) {
		uart_echo = 1;
	} else {
		uart_echo = 0;
	}

	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	/* 8-bit data */ 
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	/* Enable RX and TX */  
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	/* Enable recieve interrupt */
	UCSR0B |= _BV(RXCIE0);

	sei();  
}

int uart_putchar(char c, FILE *stream) {

	int write_pointer = (tx_buffer.end + 1) % UART_TX_BUFFER_SIZE;

	/*
	 * Check to see if we have space for another character in the buffer.
	 * If we don't, the write_pointer will now point to the end - i.e. we
	 * will be over-writing the first char to send. Wait for state to
	 * change.
	 */
	while (write_pointer == tx_buffer.start);

	tx_buffer.buffer[tx_buffer.end] = c;
	tx_buffer.end = write_pointer;

	/* Data available. Enable the transmit interrupt for serial port 0. */
	UCSR0B |= _BV(UDRIE0);

	return (int) c;
}

ISR(USART_UDRE_vect){
	/* Transmit next byte if data available in ringbuffer. */
	if (tx_buffer.start != tx_buffer.end) {
		UDR0 = tx_buffer.buffer[tx_buffer.start];
		tx_buffer.start = (tx_buffer.start + 1) % UART_TX_BUFFER_SIZE;
	} else {
		/*
		 * Nothing to send. Disable the transmit interrupt for serial
		 * port 0.
		 */
		UCSR0B &= ~_BV(UDRIE0);
	}
}

int uart_getchar(FILE *stream) {
	int ret;
	int read_pointer = rx_buffer.end;

	/* Return EOF if we have an empty buffer */
	while (rx_buffer.start == read_pointer)
		read_pointer = rx_buffer.end;

 	ret = rx_buffer.buffer[rx_buffer.start];
	rx_buffer.start = (rx_buffer.start + 1) % UART_RX_BUFFER_SIZE;
	    
	return ret;
}

ISR(USART_RX_vect) {
	int read_pointer = (rx_buffer.end + 1) % UART_RX_BUFFER_SIZE;

	/* Add next byte to ringbuffer if it has space available. */
	if (read_pointer != rx_buffer.start) {
		rx_buffer.buffer[rx_buffer.end] = UDR0;
		if (uart_echo == 1)
			uart_putchar(rx_buffer.buffer[rx_buffer.end], &uart_stream);

		rx_buffer.end = read_pointer;
	}
}

