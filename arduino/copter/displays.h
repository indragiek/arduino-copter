// ArduinoCopter
// displays.h
//
// Created November 19, 2013
//
// Definitions for display-independent types and display initialization.

#ifndef __displays_h__
#define __displays_h__

// Uncomment to use the UTFT library instead of Adafruit ST7735
#define USE_UTFT 

// Uncomment to use the Adafruit RA8875 driver instead of the Adafruit
// ST7735. This is only valid when USE_TFT is not defined.
#define USE_RA8875

#ifdef USE_RA8875
	#include <Adafruit_RA8875.h>
	#define TFT_TYPE Adafruit_RA8875
#else

	#ifdef USE_UTFT
		#include <UTFT.h>
		#define TFT_TYPE UTFT
	#else
		#include <Adafruit_ST7735.h>
		#define TFT_TYPE Adafruit_ST7735
	#endif
#endif


#endif