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

	int header = Serial3.read();
	if (header == 0x01 && callbacks.button) { // Button up/down command
		if (num_bytes < 2) return;
		BTButtonState state = (BTButtonState)Serial3.read();
		callbacks.button(state);
	} else if (header == 0x02 && callbacks.toggle) {
		callbacks.toggle();
	}
}

void bt_receiver_send_reset() {
	Serial3.write(0x03);
}

void bt_receiver_increment_score() {
	Serial3.write(0x04);
}
