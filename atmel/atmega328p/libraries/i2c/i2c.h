#ifndef I2C_H
#define I2C_H

#define I2C_FREQ_100 1
#define I2C_FREQ_400 2

#define I2C_IO 5
#define I2C_AGAIN 11
#define I2C_BUSY 16

/*
 * Saved TWI status register, for error messages only.  We need to
 * save it in a variable, since the datasheet only guarantees the TWSR
 * register to have valid contents while the TWINT bit in TWCR is set.
 */
uint8_t twi_stat;

/*
 * Error number set on error.
 */
uint8_t i2c_errno;

/*
 * Initialization of TWI (two wire interface) that is used for i2c comms.
 *
 * Param:
 *  speed : Set speed for i2c comms (I2C_FREQ_100 = 100kHz, I2C_FREQ_400 = 400kHz)
 */
void i2c_init(char);

int8_t i2c_read(uint8_t, uint8_t *, int8_t);

int8_t i2c_write(uint8_t, uint8_t *, int8_t);

int8_t i2c_pread(uint8_t, uint8_t *, int8_t, uint8_t);

int8_t i2c_pwrite(uint8_t, uint8_t *, int8_t, uint8_t);

void i2c_error(void);

#endif /* I2C_H */
