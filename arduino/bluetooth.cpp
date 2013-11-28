// ArduinoCopter
// bluetooth.cpp
//
// Game controls and score transfer over Bluetooth.
//

#include "bluetooth.h"
#include <ble_shield.h>

static BTCallbackFunctions callbacks;

// =========== Public API ============
// All Public APIs are documented in bluetooth.

void bluetooth_init(BTCallbackFunctions functions) {
	callbacks = functions;
	ble_set_pins(3, 2);
	ble_begin();
}

void bluetooth_update() {
	ble_do_events();
	if (!ble_connected()) return;

	unsigned char num_bytes = ble_available();
	if (num_bytes == 0) return;

	int header = ble_read();
	if (header == 0x01 && callbacks.button) { // Button up/down command
		if (num_bytes < 2) return;
		BTButtonState state = (BTButtonState)ble_read();
		callbacks.button(state);
	} else if (header == 0x02 && callbacks.toggle) {
		callbacks.toggle();
	}
}

void bluetooth_send_reset() {
	ble_write(0x03);
	ble_do_events();
}

void bluetooth_increment_score() {
	ble_write(0x04);
	ble_do_events();
}
