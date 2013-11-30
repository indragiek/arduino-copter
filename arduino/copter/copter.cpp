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

// Buttons and Lights
const int BTN 		= 9;
const int LED 		= 4;

// =========== Global Variables ============

#ifdef USE_LARGE_LCD
Adafruit_RA8875 tft = Adafruit_RA8875(TFT_CS, TFT_RST);
#else
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
#endif

scene *s = NULL;
boolean remote_btn_state = false; // State of the remotely controlled copter button.
boolean remote_pause_state = false; // State of the remotely controlled play/pause button (true if paused).

// =========== Function Definitions ============ 

// Shows the introduction screen with the game title, etc.
static void show_intro();

// Runs the Copter game until the player loses.
static void run_game();

// Shows the Game Over screen.
//
// @param score The game score to show.
static void game_over(long score);

// Show flashing text until the action button is pressed.
//
// @param s 		The text to flash.
// @param p 		Point at which to draw the text.
// @param size 		The text size.
// @param color 	The text color.
static void flash_action_text(const char *s, g_point p, int size, int color);

// Returns whether the button is pressed (either in hardware or
// through the Bluetooth controller)
static boolean is_button_down();

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
	pinMode(LED, OUTPUT);
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
	flash_action_text("Press button to\n        begin.", (g_point){20, 120}, 1, TFT_GREEN);
}

static void run_game() {
	if (s != NULL) {
		Serial.println("Freeing old scene");
		scene_free(s);
		s = NULL;
	}

	scene_colors colors;
	colors.terrain = TFT_GREEN;
	colors.background = TFT_BLACK;
	colors.blocks = TFT_YELLOW;
	colors.copter = TFT_WHITE;

	Serial.println("Creating new scene");
#ifdef USE_LARGE_LCD

	// We use a manual size override when testing on a large LCD because
	// the library returns the incorrect size.
	s = scene_new(&tft, TFT_SIZE, 200, 1, 125, (g_size){10, 25}, colors);
#else
	g_size tft_size = (g_size){tft.width(), tft.height()};
	s = scene_new(&tft, tft_size, 100, 1, 75, (g_size){10, 25}, colors);
#endif

	bt_receiver_send_reset();

	remote_pause_state = false;
	remote_btn_state = false;
	boolean collision = false;
	long score = 0;

	while (collision == false) {
		bt_receiver_update();
		if (remote_pause_state == false) {
			boolean btn_down = is_button_down();
			collision = scene_update(s, btn_down ? copter_up : copter_down);
			digitalWrite(LED, btn_down ? HIGH : LOW);

			bt_receiver_increment_score();
			score++;
		}
	}
	game_over(score);
}

static void game_over(long score) {
	// Draw the Game Over title
	tft.fillScreen(TFT_BLACK);
	tft.setCursor(10, 40);
	tft.setTextSize(2);
	tft.setTextColor(TFT_RED);
	tft.print("Game Over");

	// Draw the score
	tft.setCursor(12, 80);
	tft.setTextSize(1);
	tft.setTextColor(TFT_WHITE);
	tft.print("Score: ");
	tft.print(score);

	// Draw the text for retry
	flash_action_text("Press button to\n        retry.", (g_point){20, 120}, 1, TFT_GREEN);
	run_game();
}

static boolean is_button_down() {
	return remote_btn_state || (digitalRead(BTN) == LOW);
}

static void flash_action_text(const char *s, g_point p, int size, int color) {
	boolean visible = true;
	while (is_button_down() == false) {
		if (visible) {
			tft.setCursor(p.x, p.y);
			tft.setTextColor(color);
			tft.setTextSize(size);
			tft.print(s);
		} else {
			tft.fillRect(p.x, p.y, tft.width() - p.x, tft.height() - p.y, TFT_BLACK);
		}
		visible = !visible;
		delay(700);
	}
}

void bt_button_press(BTButtonState state) {
	remote_btn_state = (state == BTButtonDown) ? true : false;
}

void bt_toggle_pause() {
	remote_pause_state = !remote_pause_state;
}

void loop() {}
