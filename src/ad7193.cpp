// #include <stdlib.h>
#include "ad7193.hpp"
#include <Arduino.h>
#include <SPI.h>

#define SPI_CLK			1'000'000

static const SPISettings SPI_Settings(SPI_CLK, MSBFIRST, SPI_MODE3);

static void spi_begin(void);
static void spi_end(void);

/* Start SPI with the settings and CS pin low*/
static void spi_begin() {
	SPI.beginTransaction(SPI_Settings);
	digitalWrite(AD7193_CS, LOW);
}

/* End SPI with and CS pin high */
static void spi_end() {
	digitalWrite(AD7193_CS, HIGH);
	SPI.endTransaction();
}

/*
	Write 24-bit data to a register 
	regAdr - use AD7193_ADR
 */
void writeReg(uint8_t regAdr, uint32_t data) {
	spi_begin();
	SPI.transfer(AD7193_ADR_WRITE(regAdr));	// Send the register in WriteMode
	for (int i = 2; i >= 0; i--) {
		uint32_t temp = 0;
		/* Masked */
		temp = data & (0xFF << (i * 8));
		/* Shifted to 1 byte */
		temp = temp >> (i * 8);
		SPI.transfer(temp);
	}
	spi_end();
}

void writeReg(uint8_t regAdr, uint8_t data) {
	spi_begin();
	SPI.transfer(AD7193_ADR_WRITE(regAdr));	// Send the register in WriteMode
	SPI.transfer(data);
	spi_end();
}

// 24-bit reading from register
uint32_t readReg_24bit(uint8_t regAdr) {
	uint32_t data = 0;
	spi_begin();
	SPI.transfer(AD7193_ADR_READ(regAdr));          // Send out reg add 
	for (int i = 2; i >= 0; i--) {
		data |= (SPI.transfer(0) << (i*8));
	}
	spi_end();

	return data;
}

// 24-bit reading from register
uint8_t readReg_8bit(uint8_t regAdr) {
	uint8_t data = 0;
	spi_begin();
	SPI.transfer(AD7193_ADR_READ(regAdr));          // Send out reg add 
	data = SPI.transfer(0);
	spi_end();
	
	return data;
}

/*
	By default, it will start the continous reading of data
	If stop is true, it will end the continous reading of data

	Better name?
 */
void continous_read_start() {
	spi_begin();
	const uint8_t regAdr = AD7193_ADR_READ(AD7193_ADR(AD7193_REG_DATA));
	SPI.transfer(regAdr | (1 << AD7193_ADR_CONTINOUS_POS));
}

void continous_read_end() {
	const uint8_t regAdr = AD7193_ADR_READ(AD7193_ADR(AD7193_REG_DATA));
	SPI.transfer(regAdr);
	spi_end();
}

uint32_t readContinousWithStatus() {
	uint32_t data = 0;
	for (int i = 3; i >= 0; i--) {
		data = data | (SPI.transfer(0) << (i*8));
	}

	return data;
}

/*
	Checks if MISO/RDY_ is LOW
	Used to tell if data is ready in Continous Mode
	Returns true if data is ready
 */
bool readyToRead() {
	return !digitalRead(AD7193_MISO);
}

// Reset ADC
void resetADC() {
	spi_begin();
	uint8_t tempBuffer[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // All high for at least 40 cycles
	SPI.transfer(tempBuffer, sizeof(tempBuffer));
	spi_end();
}