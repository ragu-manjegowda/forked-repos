/*
 * uexpect - provides very basic "expect" style functionality on a stream
 *
 * uexpect doesn't do regluar expressions, just matching one or more strings
 * from data comming in via a stream. It is targetted at very space and
 * processing restricted platforms such as micro controllers.
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

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include "uexpect.h"

char uexpect_before[UEXPECT_BUF_SIZE];

int8_t uexpect(FILE *stream, const char *str1, ...)
{
	va_list strings;
	char *write_ptr;
	char *cmp_ptr;
	int16_t count = 0;
	int8_t index;
	const char *pattern;
	int16_t size;

	/* Sort out "before" buffer */
	write_ptr = uexpect_before;
	cmp_ptr = uexpect_before;
	memset(uexpect_before, '\0', UEXPECT_BUF_SIZE);

	/*
	 * Protect against overflowing "before" buffer - instead, enable return
	 * to user as a kind of timeout.
	 */
	while (count < UEXPECT_BUF_SIZE) {

		/* Get next character from stream */
		*write_ptr = getc(stream);
		write_ptr++;

		index = 0;
		/* Loop through each pattern */
		va_start(strings, str1); 

		for (pattern = str1; pattern != NULL; pattern = va_arg(strings, const char *)) {
			/* Work out pointer for comparison */
			size = strlen(pattern);

			if((write_ptr - size) < uexpect_before)
				cmp_ptr = uexpect_before;
			else
				cmp_ptr = write_ptr - size;

			if (strncmp(pattern, cmp_ptr, size) == 0) {
				va_end(strings);

				/* Terminate string with '/0' so we don't see
				 * the pattern in uexpect_before
				 */
				*cmp_ptr = '\0';

				return index;
			}

			index++;
		}

		va_end(strings);

		count++;
	}

	return -1;
}

