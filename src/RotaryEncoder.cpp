/*
 * RotaryEncoder.cpp
 * Copyright 2019 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#include <Arduino.h>

#include "RotaryEncoder.h"


/**
 * Inicializa el modo de los pines
 * correspondientes al encoder: CLK, DATA y SWITCH
 */
void RotaryEncoder::begin(uint8_t pclkPin, uint8_t pdataPin, uint8_t pswitchPin) {

  clkPin    = pclkPin;
  dataPin   = pdataPin;
  switchPin = pswitchPin;

  pinMode(clkPin, INPUT_PULLUP);
  pinMode(dataPin, INPUT_PULLUP);

  if( switchPin > 0 )
    pinMode(switchPin, INPUT_PULLUP);

  lastClkPinLevel = digitalRead(clkPin);

  switchTimestamp = 0;

  savedEvent = NONE;

}


void RotaryEncoder::begin(uint8_t pclkPin, uint8_t pdataPin) {

  begin(pclkPin, pdataPin, 0);

}


uint8_t RotaryEncoder::getEvent() {

  /*
   * Limite de tiempo en milisegundos utilizado
   * para determinar si el switch es retenido
   */
  #define HELDED_LIMIT 700

  /*
   * Almacenara el timestamp (milis()) del momento
   * en que se presiona el switch a fin de determinar
   * si la accion sera click o retencion
   */
  //static unsigned long switchTimestamp = 0;

  /*
   * Almacenara la ultima accion sucedida.
   * Se utiliza para derterminar el evento del switch
   * ya que el mismo se retorna al liberarlo y podra ser
   * click o retencion, de acuerdo al tiempo
   */
  //static uint8_t savedEvent = NONE;

  //unsigned long currTimestamp;
  uint8_t clkPinLevel, event;

  // Setea la accion "ninguna" por defecto
  event = NONE;

  // Lee el nivel del pin CLK
  clkPinLevel = digitalRead(clkPin);

  // Detecta el flanco de bajada en el pin CLK
  if ( clkPinLevel == 0 && lastClkPinLevel == 1 ) {
    /*
     * Si el nivel en el pin DATA es
     * alto indica que esta girando
     * hacia la izquierda (antihorario)
     *
     */
    if ( digitalRead(dataPin) == 1 )
      savedEvent = LEFT_TURN;
    else // caso contrario es hacia la derecha (horario)
      savedEvent = RIGHT_TURN;
  }

  lastClkPinLevel = clkPinLevel;

  if ( switchPin != 0 && !digitalRead(switchPin) ) {

    /*
     * En principio se establece que
     * la accion sera CLICK
     */
    savedEvent = SWITCH_CLICK;

    /*
     * Luego, si el tiempo de retencion supero
     * el limite HELDED_LIMIT, se determina
     * que la accion debera ser retencion
     *
     */
    if ( (millis() - switchTimestamp) > HELDED_LIMIT )
      savedEvent = SWITCH_HELD;

  }
  else {
    switchTimestamp = millis();
    event = savedEvent;
    savedEvent = NONE;
  }

  return event;

}
