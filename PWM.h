#pragma once

#include <wiringPiI2C.h>
#include <time.h>
#include <math.h>

// Copied from https://github.com/4ndr3w/PiBot . Many thanks!

enum PWMConstant {
    __MODE1              = 0x00,
    __MODE2              = 0x01,
    __SUBADR1            = 0x02,
    __SUBADR2            = 0x03,
    __SUBADR3            = 0x04,
    __PRESCALE           = 0xFE,
    __LED0_ON_L          = 0x06,
    __LED0_ON_H          = 0x07,
    __LED0_OFF_L         = 0x08,
    __LED0_OFF_H         = 0x09,
    __ALL_LED_ON_L       = 0xFA,
    __ALL_LED_ON_H       = 0xFB,
    __ALL_LED_OFF_L      = 0xFC,
    __ALL_LED_OFF_H      = 0xFD,


    __RESTART            = 0x80,
    __SLEEP              = 0x10,


    __ALLCALL            = 0x01,
    __INVRT              = 0x10,
    __OUTDRV             = 0x04,
};


void initPWM(int address = 0x40);
void setPWMFreq(int freq);
void setPWM(int channel, int on, int off);
void setAllPWM(int on, int off);

