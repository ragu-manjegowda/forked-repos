# nunchuk
A Linux I2C driver for the Wii nunchuk running on the Beaglebone Black. It
supports the following through the Linux input subsystem:
- Z button
- C button
- Joystick (X and Y)
- Accelerometer (X, Y, Z)

This driver is based on the lab from Free Electrons:

http://free-electrons.com/doc/training/linux-kernel/

## Building
To build, do the following:
```
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
make nunchuk_defconfig
make
make dtbs
```
