// ArduinoCopter
// bt_receiver.cpp
//
// Game controls and score transfer over bt_receiver.
//

#include "bt_receiver.h"
#include <Arduino.h>

static BTCallbackFunctions callbacks;

// =========== Function Declarations ============

// Sends a 32-bit unsigned integer over the serial port by breaking
// it up into 4 8-bit integers.
//
// @param n The 32-bit integer to send.
static void bt_receiver_send_uint32(uint32_t n);

// =========== Public API ============
// All Public APIs are documented in bt_receiver.

void bt_receiver_init(BTCallbackFunctions functions) {
	callbacks = functions;
	Serial3.begin(9600);
}

void bt_receiver_update() {
	int num_bytes = Serial3.available();
	if (num_bytes == 0) return;

	int header = Serial3.peek();
	if (header == 0x01 && callbacks.button) { // Button up/down command
		if (num_bytes >= 2) {
			Serial3.read(); // Flush header from the Serial buffer
			callbacks.button((BTButtonState)Serial3.read());
		}
	} else if (header == 0x02 && callbacks.toggle) {
		Serial3.read();
		callbacks.toggle();
	} else {
		Serial3.read();
	}
}

void bt_receiver_send_reset() {
	Serial3.write(0x03);
}

void bt_receiver_send_score(uint32_t score) {
	Serial3.write(0x04);
	bt_receiver_send_uint32(score);
}

void bt_receiver_send_high_score(uint32_t high_score) {
	Serial3.write(0x05);
	bt_receiver_send_uint32(high_score);
}

// =========== Private API ============

static void bt_receiver_send_uint32(uint32_t n) {
	for (int i = 0; i < sizeof(uint32_t); i++) {
    	Serial3.write(lowByte(n));
    	n >>= 8;
  	}
}
