/*
 * main.cpp
 * Copyright 2019 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#include <Arduino.h>

#include "RotaryEncoder.h"
#include "MP3Player.h"
#include "LedsPanel.h"
#include "RuliBrain.h"


///////////////////////////
//
//      MAIN WHEEL
//
#define MW_CLK_PIN     16
#define MW_DATA_PIN    15
///////////////////////////

///////////////////////////
//
//    ROTARY SELECTOR
//
#define RS_SWITCH_PIN  19
#define RS_CLK_PIN     18
#define RS_DATA_PIN    17
///////////////////////////

///////////////////////////
//
//       MP3 PLAYER
//
#define MP_RX          10
#define MP_TX          11
///////////////////////////

///////////////////////////
//
//       LEDS PANEL
//
#define LP_ENABLE_PIN  8
#define LP_CLOCK_PIN   9
#define LP_DATA_PIN    12
///////////////////////////



/*
 * Objetos globales
 */
RotaryEncoder mainWheel;
RotaryEncoder rotarySelector;
MP3Player mp3Player;
LedsPanel ledsPanel;
RuliBrain ruliBrain;


// Setup function
void setup() {

  /*
   * Inicializacion de objetos globales
   */
  mainWheel.begin(MW_CLK_PIN, MW_DATA_PIN);
  rotarySelector.begin(RS_CLK_PIN, RS_DATA_PIN, RS_SWITCH_PIN);
  mp3Player.begin(MP_RX, MP_TX);
  ledsPanel.begin(LP_ENABLE_PIN, LP_CLOCK_PIN, LP_DATA_PIN);
  ruliBrain.begin(&mainWheel, &rotarySelector, &mp3Player, &ledsPanel);

}


// Main loop
void loop() {

  ruliBrain.run();

}
