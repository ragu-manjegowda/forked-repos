/*
 * Library to drive mt4181s real time clock.
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
#include "m41t81s.h"


int8_t m41t81s_init(void)
{
	uint8_t data[2];
	int8_t retval;

	retval = i2c_pread(0x68, data, 1, 0x0F);
	if (retval != 1)
		return -1;

	if (data[0] & 0x80)
		printf("Watchdog flag set\n");

	if (data[0] & 0x40)
		printf("Alarm flag set\n");

	if (data[0] & 0x10)
		printf("Battery low\n");

	if (data[0] & 0x4)
		printf("Oscillator fail flag\n");

	retval = i2c_pread(0x68, data, 1, 0x0C);
	if (retval != 1)
		return -1;

	if (data[0] & _BV(M41T81S_HT)) {
		printf("Clearing Halt Bit");

		data[1] = data[0] & ~_BV(M41T81S_HT);
		data[0] = 0x0C;

	        retval = i2c_write(0x68, data, 2);		
	        if (retval != 2) {
			printf("Clearing Halt failed");
	                return -1;
		}

	}
	
	retval = i2c_pread(0x68, data, 1, 0x01);
	if (retval != 1)
		return -1;

	if (data[0] & _BV(M41T81S_ST)) {
		printf("Clearing Stop Bit");

		data[1] = data[0] & ~_BV(M41T81S_ST);
		data[0] = 0x01;

	        retval = i2c_write(0x68, data, 2);		
	        if (retval != 2) {
			printf("Clearing Stop failed");
	                return -1;
		}

	}
	
	return 0;
}


int8_t m41t81s_gettime(int8_t *hour, int8_t *minute, int8_t *second)
{
	int8_t retval;
	uint8_t data[3];

	retval = i2c_pread(0x68, data, 3, 0x01);
	if (retval != 3)
		return -1;

	*second = ((data[0]>>4) & 0x7) * 10 + (data[0] & 0xf);
	*minute = ((data[1]>>4) & 0x7) * 10 + (data[1] & 0xf);
	*hour = ((data[2]>>4) & 0x3) * 10 + (data[2] & 0xf);

	return 0;
}


int8_t m41t81s_settime(int8_t hour, int8_t minute, int8_t second)
{
	int8_t retval;
	uint8_t data[3];
	uint8_t current[3];

	if (second > 59)
		return -1;

	if (minute > 59)
		return -1;

	if (hour > 23)
		return -1;

	retval = i2c_pread(0x68, current, 3, 0x01);
	if (retval != 3)
		return -1;

	data[0] = (current[0] & 0x8) | ((second / 10) << 4) | (second % 10);
	data[1] = ((minute / 10) << 4) | (minute % 10);
	data[2] = (current[2] & 0xc0) | ((hour / 10) << 4) | (hour % 10);

	retval = i2c_pwrite(0x68, data, 3, 0x01);
	if (retval != 3)
		return -1;

	return 0;
}


int8_t m41t81s_getdate(int16_t *year, int8_t *month, int8_t *day)
{
	int8_t retval;
	uint8_t data[3];

	retval = i2c_pread(0x68, data, 3, 0x05);
	if (retval != 3)
		return -1;

	*day = ((data[0]>>4) & 0x3) * 10 + (data[0] & 0xf);
	*month = ((data[1]>>4) & 0x1) * 10 + (data[1] & 0xf);
	/* Only deal with 2000 to 2099 for now. */
	*year = 2000 + (data[2]>>4) * 10 + (data[2] & 0xf);

	return 0;
}


int8_t m41t81s_setdate(int16_t year, int8_t month, int8_t day)
{
	int8_t retval;
	uint8_t data[3];

	if (day > 31)
		return -1;

	if (month > 12)
		return -1;

	/* Only deal with 2000 to 2099 for now. */
	if (year > 2099)
		return -1;

	if (year < 2000)
		return -1;

	year = year - 2000;

	data[0] = ((day / 10) << 4) | (day % 10);
	data[1] = ((month / 10) << 4) | (month % 10);
	data[2] = ((year / 10) << 4) | (year % 10);

	retval = i2c_pwrite(0x68, data, 3, 0x05);
	if (retval != 3)
		return -1;

	return 0;
}


int8_t m41t81s_getdow(int8_t *dow)
{
	int8_t retval;
	uint8_t data;

	retval = i2c_pread(0x68, &data, 1, 0x04);
	if (retval != 1)
		return -1;

	*dow = data & 0x7;

	return 0;
}


int8_t m41t81s_getalarm(int8_t *month, int8_t *day, int8_t *hour, int8_t *minute, int8_t *second, int8_t *mode)
{
	int8_t retval;
	uint8_t data[5];
	int8_t temp = 0;

	retval = i2c_pread(0x68, data, 5, 0x0A);
	if (retval != 3)
		return -1;

	*month = ((data[0]>>4) & 0x1) * 10 + (data[0] & 0xf);
	*day = ((data[1]>>4) & 0x3) * 10 + (data[1] & 0xf);
	*hour = ((data[2]>>4) & 0x3) * 10 + (data[2] & 0xf);
	*minute = ((data[3]>>4) & 0x7) * 10 + (data[3] & 0xf);
	*second = ((data[4]>>4) & 0x7) * 10 + (data[4] & 0xf);

	/* Build RPT data */
	temp = ((data[1] & 0x40) >> 2) | ((data[1] & 0x80) >> 4) | ((data[2] & 0x80) >> 5) | ((data[3] & 0x8) >> 6) | ((data[4] & 0x8) >> 7);

	*mode = 0;

	switch(temp) {
		case 0x0:
			/* Match all */
			*mode = *mode | M41T81S_ALARM_PER_YEAR;
			break;
		case 0x10:
			/* Match to day, hour, minute and second */
			*mode = *mode | M41T81S_ALARM_PER_MONTH;
			break;
		case 0x18:
			/* Match to hour, minute and second */
			*mode = *mode | M41T81S_ALARM_PER_DAY;
			break;
		case 0x1C:
			/* Match to minute and second */
			*mode = *mode | M41T81S_ALARM_PER_HOUR;
			break;
		case 0x1E:
			/* Match second */
			*mode = *mode | M41T81S_ALARM_PER_MIN;
			break;
		case 0x1F:
			/* Goes off every second???  */
			*mode = *mode | M41T81S_ALARM_PER_SEC;
			break;
		default:
			return -1;
	}

	if (data[0] & 0x20)
		*mode = *mode | M41T81S_ALARM_ON_BAT;

	if (data[0] & 0x80)
		*mode = *mode | M41T81S_ALARM_INT;

	return 0;
}


int8_t m41t81s_setalarm(int8_t month, int8_t day, int8_t hour, int8_t minute, int8_t second, int8_t mode)
{
	int8_t retval;
	uint8_t data[5];

	if (month > 12)
		return -1;

	if (day > 31)
		return -1;

	if (hour > 23)
		return -1;

	if (minute > 59)
		return -1;

	if (second > 59)
		return -1;

	data[0] = ((month / 10) << 4) | (month % 10);
	data[1] = ((day / 10) << 4) | (day % 10);
	data[2] = ((hour / 10) << 4) | (hour % 10);
	data[3] = ((minute / 10) << 4) | (minute % 10);
	data[4] = ((second / 10) << 4) | (second % 10);


	if (mode & M41T81S_ALARM_PER_MONTH)
		data[1] = data[1] | 0x40;

	if (mode & M41T81S_ALARM_PER_DAY) {
		data[1] = data[1] | 0xC0;
	}

	if (mode & M41T81S_ALARM_PER_HOUR) {
		data[1] = data[1] | 0xC0;
		data[2] = data[2] | 0x80;
	}

	if (mode & M41T81S_ALARM_PER_MIN) {
		data[1] = data[1] | 0xC0;
		data[2] = data[2] | 0x80;
		data[3] = data[3] | 0x80;
	}

	if (mode & M41T81S_ALARM_PER_SEC) {
		data[1] = data[1] | 0xC0;
		data[2] = data[2] | 0x80;
		data[3] = data[3] | 0x80;
		data[4] = data[4] | 0x80;
	}

	if (mode & M41T81S_ALARM_ON_BAT)
		data[0] = data[0] | 0x20;

	if (mode & M41T81S_ALARM_INT)
		data[0] = data[0] | 0x80;

	retval = i2c_pwrite(0x68, data, 5, 0x0A);
	if (retval != 5)
		return -1;

	return 0;
}
