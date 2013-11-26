#include "scene.h"

// =========== Pin Configuration ============

// SD Card and TFT Display
const int SD_CS 	= 5;
const int TFT_CS 	= 6;
const int TFT_DC	= 7;
const int TFT_RST	= 8;

// Buttons
const int BTN 		= 9;

// =========== Global Variables ============

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST); // TFT Display
scene *s; // Main scene

// =========== Function Definitions ============ 

// Shows the introduction screen with the game title, etc.
void show_intro();

// Runs the Copter game until the player loses.
void run_game();

// =========== Function Implementations ============

void setup() {
	Serial.begin(9600);
	randomSeed(analogRead(0));
	tft.initR(INITR_BLACKTAB);
	pinMode(BTN, INPUT);	
	digitalWrite(BTN, HIGH);

	show_intro();
	run_game();
}

void show_intro() {
	tft.fillScreen(ST7735_BLACK);

	// Draw the game title "Copter"
	tft.setCursor(12, 40);
	tft.setTextSize(3);
	tft.setTextWrap(true);
	tft.print("Copter");

	// Draw the author's names
	tft.setCursor(12, 80);
	tft.setTextSize(1);
	tft.print("By Indragie Karuna\n  ratne & Jiawei Wu");

	// Draw the flashing "press button" text until
	// the user pushes the button.
	boolean visible = true;
	while (digitalRead(BTN) == HIGH) {
		if (visible) {
			tft.setCursor(20, 120);
			tft.setTextColor(ST7735_GREEN);
			tft.print("Press button to\n        begin.");
		} else {
			tft.fillRect(20, 120, 108, 40, ST7735_BLACK);
		}
		visible = !visible;
		delay(700);
	}
}

void run_game() {
	scene_colors colors;
	colors.terrain = ST7735_GREEN;
	colors.background = ST7735_BLACK;
	colors.blocks = ST7735_YELLOW;

	s = scene_new(&tft, 100, 1, 125, (g_size){10, 25}, colors);
	while (1) {
		scene_update(s, copter_up);
	}
}

void loop() {}
