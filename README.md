## ArduinoCopter
### Indragie Karunaratne & Jiawei Wu

### Project Directory Structure

Wiring diagrams and board images for both the basic and optional Bluetooth-enabled configurations are provided in the root of the project directory.

- **arduino**
	- **copter** - Game code to be run on primary Arduino
	- *bt_transmitter* - Bluetooth code to be run on secondary Arduino
	- *lib*
		- *BLEShield* - Libraries for RedBearLab BLE shield.
- *ios*
	- *CopterControl* - Source code for the iOS controller app.
	- *External* - Once git submodules have been 	initialized and cloned, contains 3rd party 
	dependencies used in the iOS project (see below)
	
- *fritzing* - Diagram source files for use with [Fritzing](http://fritzing.org/) software.

The project directory is a git repository. If you plan on using the iOS app, initialize and clone git submodules before attempting to build the project:

    cd <project dir>
    git submodule update --init --recursive

The libraries needed to run the Bluetooth board can be found in **arduino/lib** or downloaded directly from the manufacturer's web site.

### Parts

#### Required

* Arduino Mega 2560
* Adafruit 1.8" TFT Display
* LED
* Push button

#### Optional (for Bluetooth Controller)

* Secondary Arduino board (e.g. Arduino Uno)
* [RedBearLab BLE Shield v2](http://redbearlab.com/bleshield/)
* iPhone 5 or 5S running iOS 6/7
* Apple Developer Tools/Xcode running on a Mac and iOS Developer Membership
* Our CopterControl iOS software

### Connections

#### 1. TFT Display

1. GND to BB GND bus
2. VCC to BB positive bus
3. RESET to Pin 8
4. D/C (Data/Command) to Pin 7
5. TFT_CS (TFT/screen Chip Select) to Pin 6
6. MOSI (Master Out Slave In) to Pin 51
7. SCK (Clock) to Pin 52
8. MISO (Master In Slave Out) to 50
9. LITE (Backlite) to BB positive bus

#### 2. Push Button

Ground and connect to pin 9 on the Arduino.

#### 3. LED

Connect long lead to pin 4 on the Arduino and the short lead to a Green-Blue-Brown resistor connected to ground.

#### 4. Play

Upload the **copter** program (from the **/arduino/copter** folder) to the Arduino.


### Bluetooth Setup (OPTIONAL)

All instructions from here forwards will assume that the second Arduino board being used to run the Bluetooth stack is an Arduino Uno. Other Arduino boards can be used, but these instructions may need to be modified.

#### 1. Arduino & BLE Shield

1. Attach the RedBearLab BLE (Bluetooth Low Energy) shield to the Arduino Uno.
2. Set the REQN and RDYN pins to 9 and 8, respectively.
3. Connect TX (pin 7) from the Arduino Uno to RX3 (pin 15) on the Arduino Mega 2560 and RX (pin 6) to TX3 (pin 14)
4. Upload the **bt_transmitter** program (from the **/arduino/bt_transmitter** folder) to the Uno.

#### 2. CopterControl for iOS

1. Connect iOS device, open **Xcode**, and complete the device provisioning process by signing into your iOS Developer account.
2. Open the **CopterControl** project and build & run on your connected iOS device.
3. Turh on Bluetooth on the iOS device and turn on both Arduinos. It will automatically detect and connect to the BLE board.
4. No additional configuration required! Pressing the arrow button will function exactly like pressing the hardware button on the breadboard. **BONUS:** live updating score and persistent high score tracking.

