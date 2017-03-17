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

#define M41T81S_HT 6
#define M41T81S_ST 7

#define M41T81S_ALARM_PER_YEAR 0x1
#define M41T81S_ALARM_PER_MONTH 0x2
#define M41T81S_ALARM_PER_DAY 0x4
#define M41T81S_ALARM_PER_HOUR 0x8
#define M41T81S_ALARM_PER_MIN 0x10
#define M41T81S_ALARM_PER_SEC 0x20
#define M41T81S_ALARM_ON_BAT 0x40
#define M41T81S_ALARM_INT 0x80

int8_t m41t81s_init(void);

int8_t m41t81s_gettime(int8_t *hour, int8_t *minute, int8_t *second);

int8_t m41t81s_settime(int8_t hour, int8_t minute, int8_t second);

int8_t m41t81s_getdate(int16_t *year, int8_t *month, int8_t *day);

int8_t m41t81s_setdate(int16_t year, int8_t month, int8_t day);

int8_t m41t81s_getdow(int8_t *dow);

int8_t m41t81s_getalarm(int8_t *month, int8_t *day, int8_t *hour, int8_t *minute, int8_t *second, int8_t *mode);

int8_t m41t81s_setalarm(int8_t month, int8_t day, int8_t hour, int8_t minute, int8_t second, int8_t mode);
