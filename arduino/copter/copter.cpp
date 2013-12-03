// ArduinoCopter
// copter.cpp
//
// Created November 21, 2013

#include "scene.h"
#include "bt_receiver.h"
#include "colors.h"
#include <EEPROM.h>

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
static const int TFT_CS 	= 2;
static const int TFT_RST	= 3;
static const g_size TFT_SIZE = {480, 272};
#else
static const int TFT_CS 	= 6;
static const int TFT_DC	= 7;
static const int TFT_RST	= 8;
#endif

// Buttons and Lights
static const int BTN 		= 9;
static const int LED 		= 4;

// =========== Constants ============

// Random EEPROM address used to store the high scores.
static const int EEPROM_address = 254;

// =========== Global Variables ============

#ifdef USE_LARGE_LCD
Adafruit_RA8875 tft = Adafruit_RA8875(TFT_CS, TFT_RST);
#else
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
#endif

// State of the remotely controlled copter button.
boolean remote_btn_state = false;

// State of the remotely controlled play/pause button (true if paused).
boolean remote_pause_state = false; 

// =========== Function Definitions ============ 

// Shows the introduction screen with the game title, etc.
static void show_intro();

// Runs the Copter game until the player loses.
//
// @param high_score Pointer to a high score to set and write
// to EEPROM if the player beats it.
static void run_game(uint32_t *high_score);

// Shows the Game Over screen.
//
// @param score 		The game score to show.
// @param high_score 	Pointer to the high score to show.
static void game_over(uint32_t score, uint32_t *high_score);

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

// Returns high score read from the EEPROM.
static long read_EEPROM_score();

// Writes a high score to the EEPROM.
static void write_EEPROM_score(uint32_t score);

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

	uint32_t high_score = read_EEPROM_score();
	show_intro();
	run_game(&high_score);
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

static void run_game(uint32_t *high_score) {
	// Set up a new scene using a selected set of colors.
	scene_colors colors;
	colors.terrain = TFT_GREEN;
	colors.background = TFT_BLACK;
	colors.blocks = TFT_YELLOW;
	colors.copter = TFT_WHITE;
#ifdef USE_LARGE_LCD

	// We use a manual size override when testing on a large LCD because
	// the library returns the incorrect size.
	scene *s = scene_new(&tft, TFT_SIZE, 200, 1, 125, (g_size){10, 25}, colors);
#else
	g_size tft_size = (g_size){tft.width(), tft.height()};
	scene *s = scene_new(&tft, tft_size, 100, 1, 75, (g_size){10, 25}, colors);
#endif

	// Send the reset signal to the Bluetooth receiver to let it know that 
	// a new game has started.
	bt_receiver_send_reset();

	// Reset a bunch of game-related state variables back to their initial
	// state.
	remote_pause_state = false;
	boolean collision = false;
	uint32_t score = 0;

	// Bluetooth I/O here has to be limited to avoid making the game lag. Instead
	// of sending the score on every loop iteration, we rate limit it to only send
	// every X iterations to the point where it doesn't induce any noticeable
	// lag in gameplay.
	const int score_rate_limit = 20;
	int score_rate = 0;

	// Continue updating the game scene until a collision has occurred.
	while (collision == false) {
		bt_receiver_update(); // 
		if (remote_pause_state == false) {
			boolean btn_down = is_button_down();
			collision = scene_update(s, btn_down ? copter_up : copter_down);
			digitalWrite(LED, btn_down ? HIGH : LOW); // Light up the LED according to button press.

			score++;
			score_rate++;
			if (score_rate >= score_rate_limit) {
				bt_receiver_send_score(score);
				score_rate = 0;
			}
		}
	}
	scene_free(s);

	// New high scores are written to the EEPROM where they are persisted across
	// Arduino resets. We are careful to write only when the high score has changed
	// because the EEPROM has a limit of 100,000 write/erase cycles.
	if (score > *high_score) {
		*high_score = score;
		write_EEPROM_score(*high_score);
	}

	// If the user was pressing the button when the game ended, we don't want to throw
	// them right back into another game before the game over screen has a chance to be
	// displayed, so we manually override the button state.
	remote_btn_state = false;

	// Since the score updates only X iterations when the game is ongoing, the BT receiver
	// might not have the most up to date score at the end of the game, so that's taken
	// care of here.
	bt_receiver_send_score(score);

	// Show game over screen.
	game_over(score, high_score);
}

static void game_over(uint32_t score, uint32_t *high_score) {
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
	tft.print("\n  High Score: ");
	tft.print(*high_score);

	// Draw the text for retry
	flash_action_text("Press button to\n        retry.", (g_point){20, 120}, 1, TFT_GREEN);
	run_game(high_score);
}

static boolean is_button_down() {
	// Return true when either the hardware button or the software button on the
	// Bluetooth controller is being pressed.
	return (remote_btn_state == true) || (digitalRead(BTN) == LOW);
}

static void flash_action_text(const char *s, g_point p, int size, int color) {
	boolean visible = true;
	// Number of loop iterations before the visibility of the text changes.
	// This is used instead of delay() to allow the Bluetooth receiver to update
	// quickly during the loop without blocking it.
	//
	// Unfortunately this means that the time interval at which the switch occurs
	// will vary between processor clock speeds. 
	const long blink_switch_count = 40000;
	long blink_current_count = 0;
	while (is_button_down() == false) {
		if (++blink_current_count >= blink_switch_count) {
			visible = !visible;
			blink_current_count = 0;
			if (visible) {
				tft.setCursor(p.x, p.y);
				tft.setTextColor(color);
				tft.setTextSize(size);
				tft.print(s);
			} else {
				tft.fillRect(p.x, p.y, tft.width() - p.x, tft.height() - p.y, TFT_BLACK);
			}
		}
		bt_receiver_update();
	}
}

static long read_EEPROM_score() {
	const int byte_count = sizeof(uint32_t);
  	uint8_t bytes[byte_count];
  	uint32_t value = 0;
	for (int i = 0; i < byte_count; i++) {
		uint8_t byte = EEPROM.read(EEPROM_address + i);
		if (i == 0 && byte == 255) return 0;
		bytes[i] = byte;
	}
	for (int i = (byte_count - 1); i >= 0; i--) {
		value <<= 8;;
		value |= bytes[i];
	}
	return value;
}

static void write_EEPROM_score(uint32_t score) {
	for (int i = 0; i < sizeof(uint32_t); i++) {
		EEPROM.write(EEPROM_address + i, lowByte(score));
		score >>= 8;
	}
}

void bt_button_press(BTButtonState state) {
	remote_btn_state = (state == BTButtonDown) ? true : false;
}

void bt_toggle_pause() {
	remote_pause_state = !remote_pause_state;
}

void loop() {}
