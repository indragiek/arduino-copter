// ArduinoCopter
// copter.cpp
//
// Created November 21, 2013

#include "scene.h"
#include "bt_receiver.h"

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
boolean remote_btn_state = false; // State of the remotely controlled copter button.
boolean remote_pause_state = false; // State of the remotely controlled play/pause button (true if paused).

// =========== Function Definitions ============ 

// Shows the introduction screen with the game title, etc.
static void show_intro();

// Runs the Copter game until the player loses.
static void run_game();

// Bluetooth callbacks
void bt_button_press(BTButtonState state);
void bt_toggle_pause();

// =========== Function Implementations ============

void setup() {
	Serial.begin(9600);
	randomSeed(analogRead(0));
	BTCallbackFunctions functions = (BTCallbackFunctions){&bt_button_press, &bt_toggle_pause};
	bt_receiver_init(functions);

	tft.initR(INITR_BLACKTAB);
	pinMode(BTN, INPUT);	
	digitalWrite(BTN, HIGH);

	show_intro();
	run_game();
}

static void show_intro() {
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

static void run_game() {
	if (s) free(s);

	scene_colors colors;
	colors.terrain = ST7735_GREEN;
	colors.background = ST7735_BLACK;
	colors.blocks = ST7735_YELLOW;
	colors.copter = ST7735_RED;
	s = scene_new(&tft, 100, 1, 125, (g_size){10, 25}, colors);

	bt_receiver_send_reset();
	remote_pause_state = false;
	remote_btn_state = false;
	while (1) {
		bt_receiver_update();
		if (remote_pause_state == false) {
			boolean btn_down = (digitalRead(BTN) == LOW) || remote_btn_state;
			scene_update(s, btn_down ? copter_up : copter_down);
			bt_receiver_increment_score();
		}
	}
}


void bt_button_press(BTButtonState state) {
	remote_btn_state = (state == BTButtonDown) ? true : false;
}

void bt_toggle_pause() {
	remote_pause_state = !remote_pause_state;
}

void loop() {}
