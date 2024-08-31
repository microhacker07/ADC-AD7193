#pragma once

#include <stdint.h>

#define AD7193_CHIP_ID		0x2
#define AD7193_CS		PB7
#define AD7193_MISO		PA6

/* Registers */
#define AD7193_REG_COMM		0	// WO 8-bit
#define AD7193_REG_STAT		0	// R0 8-bit
#define AD7193_REG_MODE		1	// RW 24-bit
#define AD7193_REG_CONF		2	// RW 24-bit
#define AD7193_REG_DATA		3	// RO 24-bit
#define AD7193_REG_ID		4	// RO 8-bit but only 4 LSB bits matter
#define AD7193_REG_GPOCON	5	// RW 8-bit
#define AD7193_REG_OFFSET	6	// RW 24-bit
#define AD7193_REG_FULL		7	// RW 24-bit

#define AD7193_ADR(reg_address)		(reg_address << 3)
#define AD7193_ADR_WEN				(1<<7)
#define AD7193_ADR_READ(address)	(address | 1<<6)
#define AD7193_ADR_WRITE(address)	(address & ~(1<<6))
#define AD7193_ADR_CONTINOUS_POS	2

/* Configuration register */

/* Channel */
#define AD7193_CH0		0
#define AD7193_CH1		1
#define AD7193_CH2		2
#define AD7193_CH3		3
#define AD7193_CH4		4
#define AD7193_CH5		5
#define AD7193_CH6		6
#define AD7193_CH7		7
#define AD7193_TEMP		8

#define SET_CHANNEL(channel)		(1 << channel)
#define AD7193_POLARITY				(1<<3)

enum ad7193_gain {
	AD7193_GAIN_1 = 0,
	AD7193_GAIN_8 = 3,
	AD7193_GAIN_16 = 4,
	AD7193_GAIN_32 = 5,
	AD7193_GAIN_64 = 6,
	AD7193_GAIN_128 = 7
};

void writeReg(uint8_t, uint32_t);
void writeReg(uint8_t, uint8_t);
uint32_t readReg_24bit(uint8_t);
uint8_t readReg_8bit(uint8_t);

void continous_read_start(void);
void continous_read_end(void);
uint32_t readContinousWithStatus(void);
bool readyToRead();

void resetADC(void);