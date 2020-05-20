/*
 * RuliBrain.cpp
 * Copyright 2019 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#include <Arduino.h>
#include <EEPROM.h>

#include "RuliBrain.h"


///////////////////////////
//
//    Funcionalidades
//
#define WELCOME           0
#define SIMPLE_ROULETTE   1
#define RANDOM_COLOR      2
#define FOLLOW_THE_COLOR  3
#define TURN_METER        4
#define VELOCITY_METER    5
#define CUSTOM_SHAPE      6
#define SOUND_SHOOTING    7
#define MUSIC             8
#define IDDLE             9
///////////////////////////


///////////////////////////
//
//   Intervalos definidos
//
#define SELECTOR_BLINK_INTERVAL         0
#define RANDOM_COLOR_INTERVAL           1
#define LED_SPEAKER_INTERVAL            2
#define SPINNING_DURATION_INTERVAL      3
#define FOLLOW_COLOR_BLINK_INTERVAL     4
#define VELOCITY_METER_INTERVAL         5
#define VELOCITY_METER_BLINK_INTERVAL   6
#define VOLUME_SETTING_INTERVAL         7
#define VOLUME_SETTING_BLINK_INTERVAL   8
#define CUSTOM_SHAPE_BLINK_INTERVAL     9
#define MUSIC_BLINK_INTERVAL           10
#define MUSIC_VOLUME_INTERVAL          11
#define WELCOME_INTERVAL               12
#define IDDLE_INTERVAL                 13
#define IDDLE_STARS_INTERVAL           14
#define SOUND_SHOOTING_INTERVAL        15
//
#define TOGGLE_STEPS      2
#define ON                1
#define OFF               2
///////////////////////////


#define EEPROM_INITIAL_KEY        0
#define EEPROM_INITIAL_KEY_VALUE  'R'
#define EEPROM_VOLUME             1
#define EEPROM_FUNCTION           2


/**
 * Metodo de inicializacion
 * simil constructor
 */
void RuliBrain::begin(RotaryEncoder *pmainWheel, RotaryEncoder *protarySelector, MP3Player *pmp3Player, LedsPanel *pledsPanel) {

  mainWheel      = pmainWheel;
  rotarySelector = protarySelector;
  mp3Player      = pmp3Player;
  ledsPanel      = pledsPanel;

  // Inicializacion parametros por defecto
  prevFunction    = SIMPLE_ROULETTE;
  currentFunction = WELCOME;
  selectedFunction = SIMPLE_ROULETTE;
  currentStep = 0;
  spinSound = 2;
  volume = 5;
  //

  // Inicializacion de flags
  initializeFunction = 1;
  funcSelectorIsActive = 0;
  volumeSettingIsActive = 0;
  speaking = 0;
  spinning = 0;
  //


  /*
   * Inicializacion en 0x00 de los vectores de intervalos
   * y de datos de proposito general
   */
  memset(intervals, 0x00, sizeof(Interval_t) * INTERVALS);
  memset(data, 0x00, DATA_SIZE);

  /*
   * Verificacion/resguardo de parametros
   * en memoria EEPROM
   */
  if ( EEPROM.read(EEPROM_INITIAL_KEY) != EEPROM_INITIAL_KEY_VALUE ) {
    EEPROM.write(EEPROM_INITIAL_KEY, EEPROM_INITIAL_KEY_VALUE);
    EEPROM.write(EEPROM_VOLUME, volume);
    EEPROM.write(EEPROM_FUNCTION, selectedFunction);
  } else {
    volume = EEPROM.read(EEPROM_VOLUME);
    selectedFunction = EEPROM.read(EEPROM_FUNCTION);
  }

  // Inicializacion de indicador de funcionalidad activa
  ledsPanel->setValue(FUNC_INDICATOR, (0x01 << (selectedFunction-1)) );

  // Inicializacion volumen de reproductor MP3
  mp3Player->volume(volume);

}


/**
 * Devuelve un valor entre 0 y [steps] incrementando dicho valor
 * siempre y cuando el tiempo sea >= [ms] entre cada invocacion
 * se devera especificar un indicador de intervalo [interval] que
 * puede ser de 0 a INTERVALS
 *
 */
uint8_t RuliBrain::getInterval(uint8_t interval, unsigned long ms, uint8_t steps) {

  unsigned long tstamp;
  uint8_t ret;

  tstamp = millis();

  ret = 0;

  if ( ! intervals[interval].timestamp )
    intervals[interval].timestamp = tstamp;

  if ( (tstamp - intervals[interval].timestamp) >= ms ) {

    intervals[interval].timestamp = tstamp;

    if ( intervals[interval].currentStep < steps )
      intervals[interval].currentStep++;
    else
      intervals[interval].currentStep = 0;

    ret = intervals[interval].currentStep;
  }

  return ret;

}


void RuliBrain::resetInterval(uint8_t interval) {

  intervals[interval].timestamp = millis();
  intervals[interval].currentStep = 0;

}


void RuliBrain::mp3FinishFlush(void) {
  byte aux = mp3Player->finished();
}


void RuliBrain::run(void) {

  wheelEvent = mainWheel->getEvent();
  selectorEvent = rotarySelector->getEvent();

  if ( currentFunction != WELCOME ) {

    if ( funcSelectorIsActive && selectorEvent == SWITCH_HELD )
      volumeSettingIsActive = 1;
    else if ( selectorEvent == SWITCH_HELD )
      funcSelectorIsActive = 1;

    if ( funcSelectorIsActive && volumeSettingIsActive == 0 )
      functionSelector();
  }

  if ( speaking )
    ledSpeakEffect();
  else if ( volumeSettingIsActive )
    volumeSetting();
  else {

    switch(currentFunction) {
      case WELCOME           : { welcome        (); break; }
      case SIMPLE_ROULETTE   : { simpleRoulette (); break; }
      case RANDOM_COLOR      : { randomColor    (); break; }
      case FOLLOW_THE_COLOR  : { followTheColor (); break; }
      case TURN_METER        : { turnMeter      (); break; }
      case VELOCITY_METER    : { velocityMeter  (); break; }
      case CUSTOM_SHAPE      : { customShape    (); break; }
      case SOUND_SHOOTING    : { soundShooting  (); break; }
      case MUSIC             : { music          (); break; }
    }

    if ( currentFunction != MUSIC )
      iddleCheck();

    //byte aux = mp3Player->finished();
  }

}


void RuliBrain::iddleCheck() {

  if ( wheelEvent != NONE || selectorEvent != NONE ) {

    resetInterval(IDDLE_INTERVAL);

    if ( currentFunction == IDDLE ) {
      currentFunction = prevFunction;
      ledsPanel->setValue(FUNC_INDICATOR, (0x01 << (selectedFunction-1)) );
      initializeFunction = 1;
    }

  }

  switch ( getInterval(IDDLE_INTERVAL, 60000, 8) ) {
    case 1: {
      mp3Player->stop();
      speak(1, 4);
      prevFunction = currentFunction;
      currentFunction = IDDLE;
      ledsPanel->setValue(FUNC_INDICATOR, 0 );
      break;
    }
    case 2: { mp3Player->stop(); speak(1, 5); break; }
    case 4: { mp3Player->stop(); speak(1, 6); break; }
    case 8: { mp3Player->stop(); speak(1, 7); }
  }

  if ( currentFunction == IDDLE && speaking == 0 && ledsPanel->getValue(YELLOW) )
    ledsPanel->setValue(YELLOW, 0);

  if ( currentFunction == IDDLE )
    switch ( getInterval(IDDLE_STARS_INTERVAL, 120, 8) ) {
      case 1: { ledsPanel->setWheelValues(random(0, 31), 1); break; }
      case 2: { ledsPanel->setWheelValues(0, 0, 0, 0, 0); break; }
    }

}


void RuliBrain::functionSelector() {

  switch ( getInterval(SELECTOR_BLINK_INTERVAL, 70, TOGGLE_STEPS) ) {
    case ON: { ledsPanel->setValue(FUNC_INDICATOR, 0xFF); break; }
    case OFF: { ledsPanel->setValue(FUNC_INDICATOR, (0x01 << (selectedFunction-1)) ^ 0xFF); }
  }


  switch(selectorEvent) {

    case RIGHT_TURN: { if ( selectedFunction < 8 ) selectedFunction++; break; }

    case LEFT_TURN: { if ( selectedFunction > 1 ) selectedFunction--; break; }

    case SWITCH_HELD: { ledsPanel->setValue(FUNC_INDICATOR, 0xFF); break; }

    case SWITCH_CLICK: {
      speak(selectedFunction + 1, 1);
      funcSelectorIsActive = 0;
      initializeFunction = 1;
      currentFunction = selectedFunction;
      currentStep = 0;
      ledsPanel->setValue(FUNC_INDICATOR, (0x01 << (selectedFunction-1)) );
      EEPROM.write(EEPROM_FUNCTION, selectedFunction);
      resetInterval(IDDLE_INTERVAL);
    }
  }

}


void RuliBrain::volumeSetting() {

  if ( selectorEvent == SWITCH_HELD ) {
    ledsPanel->setValue(FUNC_INDICATOR, (0x01 << (currentFunction-1)) );
    mp3Player->playFolder(1, 2);
  }

  switch ( getInterval(VOLUME_SETTING_BLINK_INTERVAL, 70, TOGGLE_STEPS) ) {
    case ON: { ledsPanel->setValue(YELLOW, 0xFF); break; }
    case OFF: { ledsPanel->setValue(YELLOW, 0x00); }
  }

  if ( selectorEvent == RIGHT_TURN || wheelEvent == RIGHT_TURN )
    mp3Player->volumeUp();
  else if ( selectorEvent == LEFT_TURN || wheelEvent == LEFT_TURN )
    mp3Player->volumeDown();

  if ( selectorEvent != NONE || wheelEvent != NONE ) {

    resetInterval(VOLUME_SETTING_INTERVAL);

    for ( uint8_t i =  0 ; i < 32 ; i++ )
      if ( i <= (mp3Player->getVolume() - 1) )
        ledsPanel->setWheelValues(i, 1, 0);
      else
        ledsPanel->setWheelValues(i, 0, 0);
  }

  if ( selectorEvent == SWITCH_CLICK || getInterval(VOLUME_SETTING_INTERVAL, 1000, 15) == 15 ) {
      volumeSettingIsActive = 0;
      funcSelectorIsActive = 0;
      initializeFunction = 1;
      EEPROM.write(EEPROM_VOLUME, mp3Player->getVolume());
  }

}


void RuliBrain::speak(uint8_t folderNumber, uint8_t fileNumber) {

  //byte aux = mp3Player->finished();

  while ( mp3Player->finished() ) ;

  speaking = 1;

  //mp3Player->stop();

  mp3Player->playFolder(folderNumber, fileNumber);

}


void RuliBrain::ledSpeakEffect(void) {

  switch ( getInterval(LED_SPEAKER_INTERVAL, 30, 8) ) {
    case 1: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0xFF, 0x00); break; }
    case 2: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x7E, 0x00); break; }
    case 3: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x3C, 0x00); break; }
    case 4: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x18, 0x00); break; }
    case 5: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x00, 0x00); break; }

    case 6: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x18, 0x00); break; }
    case 7: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x3C, 0x00); break; }
    case 8: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x7E, 0x00); break; }

  }

  if ( mp3Player->finished() )
    speaking = 0;

}


void RuliBrain::playSpinSound() {

  if ( wheelEvent == RIGHT_TURN || wheelEvent == LEFT_TURN ) {

    if ( spinning == 0 )
      mp3Player->playFolder(currentFunction + 1, spinSound);

    spinning = 1;

    resetInterval(SPINNING_DURATION_INTERVAL);

  }

  if ( spinning == 1 && getInterval(SPINNING_DURATION_INTERVAL, 300, 2) == 2 ) {
    mp3Player->stop();
    spinning = 0;
  }

}



////////////////////////////////////////////////////
////////////////////////////////////////////////////
/////                                          /////
/////        FUNCIONALIDADES DE RULI           /////
/////                                          /////
////////////////////////////////////////////////////
////////////////////////////////////////////////////

void RuliBrain::welcome() {

  #define PREV_STEP 0

  if ( initializeFunction ) {

    data[PREV_STEP] = 99;
    initializeFunction = 0;
  }

  uint8_t step = getInterval(WELCOME_INTERVAL, 22, 80);

  if ( step != data[PREV_STEP] ) {

    data[PREV_STEP] = step;

    if ( step == 1 )
      mp3Player->playFolder(1, 3);

    if ( step < 40 )
      ledsPanel->setWheelValues(step, 1);
    else
      ledsPanel->setWheelValues(step-40, 0);

    if ( step == 79 ) {
      //mp3FinishFlush();
      mp3Player->stop();
      speak(1, 1);
      currentFunction = selectedFunction;
      initializeFunction = 1;
    }

  }

}


void RuliBrain::simpleRoulette() {

  if ( initializeFunction ) {
    ledsPanel->setWheelValues(0xFF, 0x00, 0x00, 0xFF, 0x00);
    spinSound = 2;
    currentStep = 0;
    initializeFunction = 0;
  }

  playSpinSound();

  switch(wheelEvent) {

    case RIGHT_TURN: {

      ledsPanel->rotate(RIGHT);

      break;
    }

    case LEFT_TURN:  {

      ledsPanel->rotate(LEFT);

      break;
    }
  }

  if ( selectorEvent == SWITCH_CLICK ) {

    if ( currentStep < 4 )
      currentStep++;
    else
      currentStep = 0;

    switch(currentStep) {
      case 0: { ledsPanel->setWheelValues(0xFF, 0x00, 0x00, 0xFF, 0x00); spinSound = 2; break; }
      case 1: { ledsPanel->setWheelValues(0x01, 0x04, 0x08, 0x20, 0x00); spinSound = 3; break; }
      case 2: { ledsPanel->setWheelValues(0xFF, 0xFF, 0xFF, 0x00, 0x00); spinSound = 4; break; }
      case 3: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x00, 0x03); spinSound = 5; break; }
      case 4: { ledsPanel->setWheelValues(0x88, 0x88, 0x88, 0x88, 0x88); spinSound = 6; break; }
    }
  }

}


void RuliBrain::randomColor() {

  if ( initializeFunction ) {
    ledsPanel->setWheelValues(0x00, 0xff, 0x00, 0x00, 0x00);
    spinSound = 2;
    currentStep = 0;
    initializeFunction = 0;
  }

  playSpinSound();

  if ( wheelEvent == RIGHT_TURN || wheelEvent == LEFT_TURN ) {

    resetInterval(RANDOM_COLOR_INTERVAL);

    if ( currentStep != 1 ) { //== 2 ) {
      ledsPanel->setWheelValues(0x00, 0x01, 0x00, 0x00, 0x00);
      currentStep = 0;
    }

    if ( currentStep == 0 )
      currentStep = 1;

  }

  switch(wheelEvent) {
    case RIGHT_TURN: { ledsPanel->rotate(RIGHT); break; }
    case LEFT_TURN:  { ledsPanel->rotate(LEFT); break; }
  }

  if ( getInterval(RANDOM_COLOR_INTERVAL, 1000, 2) == 2 ) {

    if ( currentStep == 1 ) {
      for ( uint8_t i = BLUE ; i <= RED ; i++ )
        if ( ledsPanel->getValue(i) ) {
          ledsPanel->setValue(i, 0xff);
          mp3Player->playFolder( 3, i + 3 );
          break;
        }

      currentStep = 2;
    }

  }

}


void RuliBrain::followTheColor() {

  #define COLOR_SELECTED 0
  #define SPEACH         1

  if ( initializeFunction ) {
    data[SPEACH] = 2;
    currentStep = 0;
    initializeFunction = 0;
  }

  switch(currentStep) {

    case 0: {

      data[COLOR_SELECTED] = (byte) random(1, 6);

      speak(4, data[SPEACH]);

      if ( data[SPEACH] < 6 )
        data[SPEACH]++;
      else
        data[SPEACH] = 2;

      currentStep = 1;

      break;

    }

    case 1: {

      if ( speaking == 0 ){
        speak(4, data[COLOR_SELECTED] + 6);
        currentStep = 2;
      }

      break;

    }

    case 2: {

      if ( speaking == 0 ){
        mp3Player->playFolder( 4, 12 );
        ledsPanel->setWheelValues(0xf0, 0x0f, 0x00, 0x00, 0x00);
        currentStep = 3;
      }

      break;

    }

    case 3: {

      if ( mp3Player->finished() )
        currentStep = 0;

      if ( ledsPanel->getValue(data[COLOR_SELECTED]) == 0xFF ) {

        mp3Player->stop();

        mp3Player->playFolder( 4, 13 );

        currentStep = 4;
      }

      break;

    }

    case 4: {

      if ( mp3Player->finished() )
        currentStep = 0;

      switch ( getInterval(FOLLOW_COLOR_BLINK_INTERVAL, 40, TOGGLE_STEPS) ) {
        case ON: { ledsPanel->setValue(data[COLOR_SELECTED], 0xFF); break; }
        case OFF: { ledsPanel->setValue(data[COLOR_SELECTED], 0x00); }
      }

      break;

    }

  }


  if ( currentStep != 4 )
    switch(wheelEvent) {
      case RIGHT_TURN: { ledsPanel->rotate(RIGHT); break; }
      case LEFT_TURN:  { ledsPanel->rotate(LEFT);  break; }
    }

}


void RuliBrain::turnMeter() {

  if ( initializeFunction ) {
    ledsPanel->setWheelValues(0x00, 0x00, 0x04, 0x00, 0x00);
    initializeFunction = 0;
  }

  switch(wheelEvent) {
    case RIGHT_TURN: {
      ledsPanel->rotate(RIGHT, 1, 0);
      ledsPanel->setValueOR(WHITE, 0x04);
      spinSound = 2;
      break;
    }
    case LEFT_TURN:  {
      ledsPanel->rotate(LEFT, 1, 0);
      ledsPanel->setValueAND(WHITE, 0xF7);
      spinSound = 3;
      break;
    }
  }

  switch ( ledsPanel->getValue(WHITE) ) {
    case 0xFF: { ledsPanel->setWheelValues(0x00, 0x00, 0x04, 0x00, 0x00); break; }
    case 0x00: { ledsPanel->setWheelValues(0xFF, 0xFF, 0xFB, 0xFF, 0xFF); break; }
  }

  playSpinSound();

}


void RuliBrain::velocityMeter() {

  #define VELOCITY 0
  #define PREV_VELOCITY 1

  if ( initializeFunction ) {

    data[VELOCITY] = 0;
    data[PREV_VELOCITY] = 0;

    ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x00, 0x00);

    initializeFunction = 0;
  }

  if ( wheelEvent == RIGHT_TURN ) {
    data[VELOCITY]++;
    ledsPanel->setValue(YELLOW, 0, 0);
  }
  else if ( wheelEvent == LEFT_TURN )
    spinning = 1;

  if ( spinning )
    switch ( getInterval(VELOCITY_METER_BLINK_INTERVAL, 100, 9) ) {
      case 1: { ledsPanel->setValue(YELLOW, B11111110); break; }
      case 2: { ledsPanel->setValue(YELLOW, B11111101); break; }
      case 3: { ledsPanel->setValue(YELLOW, B11111011); break; }
      case 4: { ledsPanel->setValue(YELLOW, B11110111); break; }
      case 5: { ledsPanel->setValue(YELLOW, B11101111); break; }
      case 6: { ledsPanel->setValue(YELLOW, B11011111); break; }
      case 7: { ledsPanel->setValue(YELLOW, B10111111); break; }
      case 8: { ledsPanel->setValue(YELLOW, B01111111); break; }
      case 9: { spinning = 0; }
    }

  if ( getInterval(VELOCITY_METER_INTERVAL, 150, TOGGLE_STEPS) == ON ) {

    for ( uint8_t i =  0 ; i < 32 ; i++ )
      if ( i <= data[VELOCITY] )
        ledsPanel->setWheelValues(i, 1, 0);
      else
        ledsPanel->setWheelValues(i, 0, 0);

    if ( data[VELOCITY] != data[PREV_VELOCITY] ){
      if ( data[VELOCITY] > 0 )
        mp3Player->playFolder(6, data[VELOCITY] + 1);
      else
        mp3Player->stop();
    }

    data[PREV_VELOCITY] = data[VELOCITY];

    ledsPanel->refresh();

    data[VELOCITY] = 0;
  }

}


void RuliBrain::customShape() {

  #define CURSOR 0
  #define CURSOR_VALUE 1

  if ( initializeFunction ) {
    ledsPanel->setWheelValues(0x00, 0x00, 0x80, 0x00, 0x00);
    data[CURSOR] = 0;
    data[CURSOR_VALUE] = 1;
    spinSound = 2;
    initializeFunction = 0;
  }

  playSpinSound();

  if ( selectorEvent != NONE || wheelEvent != NONE )
    ledsPanel->setWheelValues(data[CURSOR], data[CURSOR_VALUE]);

  switch(wheelEvent) {

    case RIGHT_TURN: {

      if ( data[CURSOR] < 39 )
        data[CURSOR]++;
      else
        data[CURSOR] = 0;

      ledsPanel->rotate(RIGHT);

      break;

    }

    case LEFT_TURN: {

      if ( data[CURSOR] > 0 )
        data[CURSOR]--;
      else
        data[CURSOR] = 39;

      ledsPanel->rotate(LEFT);

    }

  }

  switch(selectorEvent) {

    case RIGHT_TURN: {

      if ( data[CURSOR] < 39 )
        data[CURSOR]++;
      else
        data[CURSOR] = 0;

      break;
    }

    case LEFT_TURN: {

      if ( data[CURSOR] > 0 )
        data[CURSOR]--;
      else
        data[CURSOR] = 39;

      break;
    }

    case SWITCH_CLICK: {

      if ( data[CURSOR_VALUE] ) {
        data[CURSOR_VALUE] = 0;
        ledsPanel->setWheelValues(data[CURSOR], 0);
      }
      else {
        data[CURSOR_VALUE] = 1;
        ledsPanel->setWheelValues(data[CURSOR], 1);
      }

      mp3Player->playFolder(7, 3);

    }

  }

  if ( selectorEvent != NONE )
    data[CURSOR_VALUE] = ledsPanel->getWheelNValue(data[CURSOR]);

  if ( spinning == 0 )
    switch ( getInterval(CUSTOM_SHAPE_BLINK_INTERVAL, 70, TOGGLE_STEPS) ) {
      case ON: { ledsPanel->setWheelValues(data[CURSOR], 1); break; }
      case OFF: { ledsPanel->setWheelValues(data[CURSOR], 0); }
    }


}


void RuliBrain::soundShooting() {

  #define SOUND_NUMBER       0
  #define LEDS_EFFECT_TYPE   1
  #define LEDS_EFFECT_ACTIVE 2

  if ( initializeFunction ) {
    ledsPanel->setWheelValues(0x00, 0x00, 0x80, 0x00, 0x00);
    data[SOUND_NUMBER]       = 0;
    data[LEDS_EFFECT_TYPE]   = 0;
    data[LEDS_EFFECT_ACTIVE] = 0;
    initializeFunction       = 0;
  }

  switch ( wheelEvent ) {
    case RIGHT_TURN: { ledsPanel->rotate(RIGHT); break; }
    case LEFT_TURN:  { ledsPanel->rotate(LEFT);  break; }
  }

  if ( selectorEvent == SWITCH_CLICK ) {

    byte soundNumber;

    for ( soundNumber = 0 ; soundNumber < 40 ; soundNumber++ )
      if ( ledsPanel->getWheelNValue(soundNumber) ) {
        data[SOUND_NUMBER] = soundNumber;
        break;
      }

    mp3Player->playFolder(8, data[SOUND_NUMBER] + 2);

    resetInterval(SOUND_SHOOTING_INTERVAL);

    data[LEDS_EFFECT_ACTIVE] = 1;

  }

  if ( data[LEDS_EFFECT_ACTIVE] ) {
    switch ( data[LEDS_EFFECT_TYPE] ) {
      case 0: {
        switch ( getInterval(SOUND_SHOOTING_INTERVAL, 50, 7) ) {
          case 1: { ledsPanel->setWheelValues(0xff, 0x00, 0x00, 0x00, 0x00); break; }
          case 2: { ledsPanel->setWheelValues(0x00, 0xff, 0x00, 0x00, 0x00); break; }
          case 3: { ledsPanel->setWheelValues(0x00, 0x00, 0xff, 0x00, 0x00); break; }
          case 4: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0xff, 0x00); break; }
          case 5: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x00, 0xff); break; }
          case 6: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x00, 0x00); break; }
          case 7: { ledsPanel->setWheelValues(data[SOUND_NUMBER], 1); data[LEDS_EFFECT_ACTIVE] = 0; }
        }
        break;
      }
      case 1: {
        switch ( getInterval(SOUND_SHOOTING_INTERVAL, 50, 7) ) {
          case 1: { ledsPanel->setWheelValues(0x10, 0x10, 0x10, 0x10, 0x10); break; }
          case 2: { ledsPanel->setWheelValues(0x18, 0x18, 0x18, 0x18, 0x18); break; }
          case 3: { ledsPanel->setWheelValues(0x3c, 0x3c, 0x3c, 0x3c, 0x3c); break; }
          case 4: { ledsPanel->setWheelValues(0x7e, 0x7e, 0x7e, 0x7e, 0x7e); break; }
          case 5: { ledsPanel->setWheelValues(0xff, 0xff, 0xff, 0xff, 0xff); break; }
          case 6: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x00, 0x00); break; }
          case 7: { ledsPanel->setWheelValues(data[SOUND_NUMBER], 1); data[LEDS_EFFECT_ACTIVE] = 0; }

        }
        break;
      }
      case 2: {
        switch ( getInterval(SOUND_SHOOTING_INTERVAL, 50, 7) ) {
          case 1: { ledsPanel->setWheelValues(0xff, 0xff, 0xff, 0xff, 0xff); break; }
          case 2: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x00, 0x00); break; }
          case 3: { ledsPanel->setWheelValues(0xff, 0xff, 0xff, 0xff, 0xff); break; }
          case 4: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x00, 0x00); break; }
          case 5: { ledsPanel->setWheelValues(0xff, 0xff, 0xff, 0xff, 0xff); break; }
          case 6: { ledsPanel->setWheelValues(0x00, 0x00, 0x00, 0x00, 0x00); break; }
          case 7: { ledsPanel->setWheelValues(data[SOUND_NUMBER], 1); data[LEDS_EFFECT_ACTIVE] = 0; }

        }
        break;
      }
      case 3: {
        switch ( getInterval(SOUND_SHOOTING_INTERVAL, 10, 22) ) {
          case 1:  { ledsPanel->setWheelValues(B10000000, B00000001, B00000000, B00000000, B00000000); break; }
          case 2:  { ledsPanel->setWheelValues(B11000000, B00000011, B00000000, B00000000, B00000000); break; }
          case 3:  { ledsPanel->setWheelValues(B11100000, B00000111, B00000000, B00000000, B00000000); break; }
          case 4:  { ledsPanel->setWheelValues(B11110000, B00001111, B00000000, B00000000, B00000000); break; }
          case 5:  { ledsPanel->setWheelValues(B11111000, B00011111, B00000000, B00000000, B00000000); break; }
          case 6:  { ledsPanel->setWheelValues(B11111100, B00111111, B00000000, B00000000, B00000000); break; }
          case 7:  { ledsPanel->setWheelValues(B11111110, B01111111, B00000000, B00000000, B00000000); break; }
          case 8:  { ledsPanel->setWheelValues(B11111111, B11111111, B00000000, B00000000, B00000000); break; }
          case 9:  { ledsPanel->setWheelValues(B11111111, B11111111, B00000001, B00000000, B10000000); break; }
          case 10: { ledsPanel->setWheelValues(B11111111, B11111111, B00000011, B00000000, B11000000); break; }
          case 11: { ledsPanel->setWheelValues(B11111111, B11111111, B00000111, B00000000, B11100000); break; }
          case 12: { ledsPanel->setWheelValues(B11111111, B11111111, B00001111, B00000000, B11110000); break; }
          case 13: { ledsPanel->setWheelValues(B11111111, B11111111, B00011111, B00000000, B11111000); break; }
          case 14: { ledsPanel->setWheelValues(B11111111, B11111111, B00111111, B00000000, B11111100); break; }
          case 15: { ledsPanel->setWheelValues(B11111111, B11111111, B01111111, B00000000, B11111110); break; }
          case 16: { ledsPanel->setWheelValues(B11111111, B11111111, B11111111, B00000000, B11111111); break; }
          case 17: { ledsPanel->setWheelValues(B11111111, B11111111, B11111111, B10000001, B11111111); break; }
          case 18: { ledsPanel->setWheelValues(B11111111, B11111111, B11111111, B11000011, B11111111); break; }
          case 19: { ledsPanel->setWheelValues(B11111111, B11111111, B11111111, B11100111, B11111111); break; }
          case 20: { ledsPanel->setWheelValues(B11111111, B11111111, B11111111, B11111111, B11111111); break; }
          case 21: { ledsPanel->setWheelValues(B00000000, B00000000, B00000000, B00000000, B00000000); break; }
          case 22: { ledsPanel->setWheelValues(data[SOUND_NUMBER], 1); data[LEDS_EFFECT_ACTIVE] = 0; }
        }
        break;
      }
    }

    if ( data[LEDS_EFFECT_ACTIVE] == 0 )
      if ( data[LEDS_EFFECT_TYPE] == 3 )
        data[LEDS_EFFECT_TYPE] = 0;
      else
        data[LEDS_EFFECT_TYPE]++;

  }

}


void RuliBrain::music() {

  byte aux;

  #define PLAYING_TRACK   0
  #define TRACK_SELECTOR  1
  #define NO_PLAYING     99

  #define TRACK_UP    if ( data[TRACK_SELECTOR] < 39 ) data[TRACK_SELECTOR]++; else data[TRACK_SELECTOR] = 0;
  #define TRACK_DOWN  if ( data[TRACK_SELECTOR] > 0 ) data[TRACK_SELECTOR]--; else data[TRACK_SELECTOR] = 39;
  #define TRACK_NEXT  if ( data[PLAYING_TRACK] < 39 ) data[PLAYING_TRACK]++; else data[PLAYING_TRACK] = 0;

  #define MP3_FINISH_FLUSH  aux = mp3Player->finished();


  if ( initializeFunction ) {
    ledsPanel->setWheelValues(0x00, 0x00, 0x80, 0x00, 0x00);
    data[PLAYING_TRACK] = 0; //NO_PLAYING;
    data[TRACK_SELECTOR] = 0;
    MP3_FINISH_FLUSH;
    mp3Player->playFolder(9, data[PLAYING_TRACK] + 2);
    initializeFunction = 0;
  }

  if ( mp3Player->finished() ) {
    ledsPanel->setWheelValues(data[PLAYING_TRACK], 0);
    TRACK_NEXT;
    mp3Player->playFolder(9, data[PLAYING_TRACK] + 2);
  }

  if ( wheelEvent != NONE )
    ledsPanel->setWheelValues(data[TRACK_SELECTOR], 0);

  switch(wheelEvent) {

    case RIGHT_TURN: { TRACK_UP; break; }

    case LEFT_TURN: { TRACK_DOWN; break; }

  }

  if ( wheelEvent != NONE )
    ledsPanel->setWheelValues(data[TRACK_SELECTOR], 1);

  if ( funcSelectorIsActive == 0 )
    switch (selectorEvent) {

      case RIGHT_TURN: { mp3Player->volumeUp(); break; }

      case LEFT_TURN: { mp3Player->volumeDown(); break; }

      case SWITCH_CLICK: {

        if ( data[PLAYING_TRACK] == data[TRACK_SELECTOR] ) {
          data[PLAYING_TRACK] = NO_PLAYING;
          mp3Player->stop();
          ledsPanel->setWheelValues(data[TRACK_SELECTOR], 1);
        }
        else {
          ledsPanel->setWheelValues(data[PLAYING_TRACK], 0);
          data[PLAYING_TRACK] = data[TRACK_SELECTOR];
          mp3Player->playFolder(9, data[PLAYING_TRACK] + 2);
        }

      }

    }

  if ( selectorEvent == RIGHT_TURN || selectorEvent == LEFT_TURN ) {

    uint8_t mask = 0x01;
    uint8_t vol = mp3Player->getVolume();

    for ( uint8_t i = 0 ; i < vol / 4 ; i++ ) {
      mask <<= 1;
      mask |= 0x01;
    }

    ledsPanel->setValue(FUNC_INDICATOR, mask);
    resetInterval(MUSIC_VOLUME_INTERVAL);

  }


  if ( data[PLAYING_TRACK] != NO_PLAYING )
    switch ( getInterval(MUSIC_BLINK_INTERVAL, 170, TOGGLE_STEPS) ) {
      case ON: { ledsPanel->setWheelValues(data[PLAYING_TRACK], 1); break; }
      case OFF: { ledsPanel->setWheelValues(data[PLAYING_TRACK], 0); }
    }

  if ( selectorEvent == SWITCH_CLICK || getInterval(MUSIC_VOLUME_INTERVAL, 1000, 5) == 5 ) {
    ledsPanel->setValue(FUNC_INDICATOR, 0x80);
    EEPROM.write(EEPROM_VOLUME, mp3Player->getVolume());
  }

}
