#include <Arduino.h>
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <Adafruit_GFX.h>    // Core graphics library
#include "scene.h"

// Display pins:
// standard U of A library settings, assuming Atmel Mega SPI pins
#define SD_CS 5 // Chip select line for SD card
#define TFT_CS 6 // Chip select line for TFT display
#define TFT_DC 7 // Data/command line for TFT
#define TFT_RST 8 // Reset line for TFT (or connect to +5V)

// TFT Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
scene *s;

void setup() {
	randomSeed(analogRead(0));
	Serial.begin(9600);
	// Initialize TFT
	tft.initR(INITR_BLACKTAB);
	scene_colors colors;
	colors.terrain = ST7735_GREEN;
	colors.background = ST7735_BLACK;

	s = scene_new(&tft, 100, 1, colors);
}

void loop() {
    scene_update(s, copter_up);
}