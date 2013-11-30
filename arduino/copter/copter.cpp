// ArduinoCopter
// copter.cpp
//
// Created November 21, 2013

#include "scene.h"
#include "bt_receiver.h"
#include "colors.h"

// Uncomment to use the large 5" LCD instead of 1.8"
// #define USE_LARGE_LCD

#ifdef USE_LARGE_LCD
#include <Adafruit_RA8875.h>
#else
#include <Adafruit_ST7735.h>
#endif

// =========== Pin Configuration ============

// SD Card and TFT Display
#ifdef USE_LARGE_LCD
const int TFT_CS 	= 2;
const int TFT_RST	= 3;
const g_size TFT_SIZE = {480, 272};
#else
const int TFT_CS 	= 6;
const int TFT_DC	= 7;
const int TFT_RST	= 8;
#endif

// Buttons
const int BTN 		= 9;

// =========== Global Variables ============

#ifdef USE_LARGE_LCD
Adafruit_RA8875 tft = Adafruit_RA8875(TFT_CS, TFT_RST);
#else
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
#endif

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

#ifdef USE_LARGE_LCD

	// The native resolution of the LCD is actually 800x480, but there's
	// a bug in the drivers that causes it not to work when this is specified
	// as the resolution. Using 480x272 works properly.
	//
	// This is also the reason why the TFT_SIZE hardcode is required (because
	// the tft.width() and tft.height() functions still return 800x480).
	tft.begin(RA8875_480x272);
	tft.displayOn(true);
	tft.GPIOX(true);
	tft.PWM1config(true, RA8875_PWM_CLK_DIV1024);
	tft.PWM1out(255);
#else
	tft.initR(INITR_BLACKTAB);
#endif
	pinMode(BTN, INPUT);	
	digitalWrite(BTN, HIGH);

	show_intro();
	run_game();
}

static void show_intro() {
	tft.fillScreen(TFT_BLACK);

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
			tft.setTextColor(TFT_GREEN);
			tft.print("Press button to\n        begin.");
		} else {
			tft.fillRect(20, 120, 108, 40, TFT_BLACK);
		}
		visible = !visible;
		delay(700);
	}
}

static void run_game() {
	if (s) free(s);

	scene_colors colors;
	colors.terrain = TFT_GREEN;
	colors.background = TFT_BLACK;
	colors.blocks = TFT_YELLOW;
	colors.copter = TFT_WHITE;
#ifdef USE_LARGE_LCD

	// We use a manual size override when testing on a large LCD because
	// the library returns the incorrect size.
	s = scene_new(&tft, TFT_SIZE, 200, 1, 125, (g_size){10, 25}, colors);
#else
	g_size tft_size = (g_size){tft.width(), tft.height()};
	s = scene_new(&tft, tft_size, 100, 1, 125, (g_size){10, 25}, colors);
#endif

	bt_receiver_send_reset();
	remote_pause_state = false;
	remote_btn_state = false;
	boolean collision = false;
	while (collision == false) {
		bt_receiver_update();
		if (remote_pause_state == false) {
			boolean btn_down = (digitalRead(BTN) == LOW) || remote_btn_state;
			collision = scene_update(s, btn_down ? copter_up : copter_down);
			bt_receiver_increment_score();
		}
	}
	tft.fillScreen(TFT_BLACK);
}


void bt_button_press(BTButtonState state) {
	remote_btn_state = (state == BTButtonDown) ? true : false;
}

void bt_toggle_pause() {
	remote_pause_state = !remote_pause_state;
}

void loop() {}
