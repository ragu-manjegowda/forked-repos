/*
 * Library to drive 1 wire devices via GPIO.
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

#ifndef OW_GPIO_H
#define OW_GPIO_H

/* 1 wire is currently connected to D5 */
#define OW_GPIO_DDR DDRD
#define OW_GPIO_PORT PORTD
#define OW_GPIO_PIN PIND
#define OW_GPIO_BIT 5

#define OW_CMD_SEARCH 0xF0
#define OW_CMD_ROM_READ 0x33
#define OW_CMD_ROM_MATCH 0x55
#define OW_CMD_ALMSRCH 0xEC

#define OW_SEARCH_START	0x0
#define OW_SEARCH_NEXT	0x1

struct ow_id {
	char family;
	char serial[6];
	char crc;
};

void ow_init(void);
int8_t ow_reset(void);
int8_t ow_read_bit(void);
int8_t ow_read(int8_t *data, int8_t count);
int8_t ow_write_bit(int8_t value);
int8_t ow_write(int8_t *data, int8_t count);
int8_t ow_valid_crc(int64_t *id);
int8_t ow_rom_search(int64_t *id, int8_t state);
int8_t ow_rom_match(int64_t *id);
int8_t ow_rom_read(int64_t *id);
int8_t ow_scratchpad_read(int8_t *data, int8_t count);

#endif /* OW_GPIO_H */
