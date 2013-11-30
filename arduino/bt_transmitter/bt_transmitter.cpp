// ArduinoCopter
// bt_receiver.cpp
//
// Created November 21, 2013
//
// Does Bluetooth I/O over Serial port.

#include <ble_shield.h>

void setup() {
	Serial.begin(9600);
	ble_begin();
}

void loop() {
	ble_do_events();
	if (!ble_connected()) return;

	while (ble_available()) {
		Serial.write(ble_read());
	}
	while (Serial.available()) {
		ble_write(Serial.read());
	}
}
