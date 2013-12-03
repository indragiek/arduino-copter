// ArduinoCopter
// bt_receiver.cpp
//
// Created November 21, 2013
//
// Does Bluetooth I/O over Serial port.

#include <ble_shield.h>
#include <SoftwareSerial.h>

SoftwareSerial BLESerial(6, 7); // RX, TX

void setup() {
	BLESerial.begin(9600);
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
