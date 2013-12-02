// ArduinoCopter
// bluetooth.h
//
// Created November 27, 2013
//
// Sends and receives Bluetooth commands over the serial port from
// another Arduino connected to the BLE shield.
//
// See bt_transmitter.h for the spec.

#ifndef __btreceiver_h__
#define __btreceiver_h__
// The two possible button press states
// BTButtonDown - When the button is pressed down.
// BTButtonUp - When the button has been released.
typedef enum {
	BTButtonUp = 0,
	BTButtonDown = 1
} BTButtonState;

// Definition for a callback function that accepts a BTButtonState as
// a parameter.
typedef void BTButtonCallback(BTButtonState state);

// Definition for a callback function that toggles the play/pause state.
typedef void BTPauseToggleCallback(void);

// Structure that defines a set of callback functions for Bluetooth
// commands.
typedef struct {
	BTButtonCallback *button;
	BTPauseToggleCallback *toggle;
} BTCallbackFunctions;

// Initializes the Bluetooth stack.
// @param functions A struct of callback functions to be called for certain Bluetooth
// 		  			commands received by the module.
//
void bt_receiver_init(BTCallbackFunctions functions);

// Checks for incoming Bluetooth data and calls the appropriate callback functions
// if necessary.
void bt_receiver_update();

// Send Bluetooth command to increment the game score by 1pt.
void bt_receiver_increment_score();

// Send Bluetooth command to indicate that the game has been reset.
void bt_receiver_send_reset();
#endif
