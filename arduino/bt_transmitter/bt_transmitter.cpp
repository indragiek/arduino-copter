// ArduinoCopter
// bt_receiver.cpp
//
// Created November 21, 2013
//
// Game controls and score transfer over Bluetooth.
// This program runs on a second Arduino and handles Bluetooth communication.

#include <ble_shield.h>

// ======== Specification ========
//
// This specification defines the communication protocol used by copter to
// exchange data between the game and an external controller device. The
// following commands are implemented:
//
// 1) RECEIVE: Button Press Down
//    Byte sequence: 0x01 0x0`
//
// 2) RECEIVE: Button Press Up
//    Byte sequence: 0x01 0x00
//
// 3) RECEIVE: Toggle play/pause
//    Byte sequence: 0x02
//
// 4) SEND: Game reset signal.
//    Byte sequence: 0x03
//
// 3) SEND: Increment score by 1pt.
//    Byte sequence: 0x04

void setup() {
	Serial3.begin(9600);
	ble_begin();
}

void loop() {
	ble_do_events();
	if (!ble_connected()) return;

	unsigned char num_bytes = ble_available();
	if (num_bytes) {
		int header = ble_read();
		if (header == 0x01 && num_bytes >= 2) { // Button up/down command
			int state = ble_read();
			Serial3.write(header);
			Serial3.write(state);
		} else {
			Serial3.write(header);
		}
	}

	if (Serial3.available()) {
		int byte = Serial3.read();
		ble_write(byte);
	}
}
