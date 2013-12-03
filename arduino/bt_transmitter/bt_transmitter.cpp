// ArduinoCopter
// bt_receiver.cpp
//
// Created November 21, 2013
//
// Does Bluetooth I/O over Serial port.

#include <ble_shield.h>
#include <SoftwareSerial.h>

static const int RX_PIN = 6;
static const int TX_PIN = 7;

SoftwareSerial BLESerial(RX_PIN, TX_PIN); // RX, TX

void setup() {
	BLESerial.begin(57600);
	ble_begin();
}

void loop() {
	ble_do_events();
	if (!ble_connected()) return;

	while (ble_available()) {
		BLESerial.write(ble_read());
	}
	while (BLESerial.available()) {
		ble_write(BLESerial.read());
	}
}
