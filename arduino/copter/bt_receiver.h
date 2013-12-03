// ArduinoCopter
// bluetooth.h
//
// Created November 27, 2013
//
// Sends and receives Bluetooth commands over the serial port from
// another Arduino connected to the BLE shield.
//
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
// 3) SEND: Update score.
//    Byte sequence: 0x04 <32 bit integer>

#ifndef __btreceiver_h__
#define __btreceiver_h__
#include <Arduino.h>

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

// Send Bluetooth command containing the game score.
void bt_receiver_send_score(uint32_t score);

// Send Bluetooth command to indicate that the game has been reset.
void bt_receiver_send_reset();
#endif
