// ArduinoCopter
// bt_receiver.cpp
//
// Game controls and score transfer over bt_receiver.
//

#include "bt_receiver.h"
#include <Arduino.h>

static BTCallbackFunctions callbacks;

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

void bt_receiver_increment_score() {
	Serial3.write(0x04);
}
