#include <Arduino.h>
#include <SPI.h>

#include "ad7193.hpp"

#if defined (USBCON) && defined(USBD_USE_CDC)
#include "USBSerial.h"
USBSerial usbserial;
#endif

uint32_t channel_data[3] = {0, 0, 0};

void setup()
{
	#if defined (USBCON) && defined(USBD_USE_CDC)
	usbserial.begin(115200);
	#endif

	pinMode(AD7193_CS, OUTPUT);
	digitalWrite(AD7193_CS, HIGH);

	SPI.begin();

	delay(5'000);
	while(!Serial) {}

	Serial.println(F("\r\n\r\nStarting ADC test"));

	/* Reboot the device */
	resetADC();
	Serial.println(F("Rebooted device"));
	delay(1);


	/* Get chip id */
	uint8_t id = 0;
	id = readReg_8bit(AD7193_ADR(AD7193_REG_ID));
	if ((id & 0xF) != AD7193_CHIP_ID) {
		Serial.println("ADC not found");
		Serial.print("Response: 0x");
		Serial.println(id & 0xF, HEX);
	} else {
		Serial.println("ADC found");
	}

	/*	Mode register
		- continous mode
		- status register appended to data
		- default clk
		- averaging by 16
		- no sinc3 filter
	 */
	const uint32_t mode = 0x1B2060;
	writeReg(AD7193_ADR(AD7193_REG_MODE), mode);
	uint32_t mode_current = readReg_24bit(AD7193_ADR(AD7193_REG_MODE));
	if (mode != mode_current) {
		Serial.println("Mode not set");
		Serial.print("Response: 0x");
		Serial.println(mode_current, HEX);
	}

	/*	Configuration register
		- pseudodifferential mode
		- ch0, ch1, ch2
		- buffer on
		- unipolar
		- gain = 1
	 */
	const uint32_t config = 0x040118;
	writeReg(AD7193_ADR(AD7193_REG_CONF), config);
	uint32_t config_curent = readReg_24bit(AD7193_ADR(AD7193_REG_CONF));
	if (config != config_curent) {
		Serial.println("Config not set");
		Serial.print("Response: 0x");
		Serial.println(config_curent, HEX);
	}

	continous_read_start();
}

void loop()
{
	if (readyToRead()) {
		uint32_t data = readContinousWithStatus();
		uint8_t channel = data & 0xF;
		data = data >> 8;
		channel_data[channel] = data;
		Serial.print(channel_data[0]);
		Serial.println();
	}
}