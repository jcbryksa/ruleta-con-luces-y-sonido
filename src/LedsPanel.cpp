/*
 * LedsPanel.cpp
 * Copyright 2019 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#include <Arduino.h>

#include "LedsPanel.h"


/*
 * Macros para manejo de salidas correspondientes
 * a los shift registers CD4064
 */
#define disableOutput() digitalWrite(enablePin, LOW)
#define enableOutput() digitalWrite(enablePin, HIGH)
#define clkPulse() digitalWrite(clkPin, LOW); digitalWrite(clkPin, HIGH)


/**
 * Inicializa el modo de los pines (output)
 * el buffer en cero (todos leds apagados) e inhabilita
 * la linea de salida de todos los shift registers
 */
void LedsPanel::begin(int penablePin, int pclkPin, int pdataPin) {

  enablePin = penablePin;
  clkPin    = pclkPin;
  dataPin   = pdataPin;

  pinMode(enablePin, OUTPUT);
  pinMode(clkPin,    OUTPUT);
  pinMode(dataPin,   OUTPUT);

  disableOutput();

  memset(ledsBuffer, 0x00, sizeof(ledsBuffer));

}


/**
 * Establece el estado encendido/apagado de
 * cada led en funcion de su estado logico
 * correspondiente en el buffer
 */
void LedsPanel::refresh(void) {

  disableOutput();

 /*
  * Recorre el buffer de leds desde el final
  * hacia el principio, en el siguiente orden:
  * RED, YELLOW, WHITE, GREEN, BLUE, FUNC_INDICATOR
  */
  for ( int i = sizeof(ledsBuffer) - 1 ; i >= 0 ; i-- ) {
   /*
    * Por cada elemento (seccion de leds) envia cada
    * uno de sus bits a la cascada de shift registers
    * comenzando por el bit mas significativo (MSB)
    */
    uint8_t mask = 0x80;
    while(mask > 0) {
      digitalWrite(dataPin, ledsBuffer[i] & mask);
      clkPulse();
      mask = mask >> 1;
      mask = mask & B01111111;
    }
  }

  enableOutput();

}


/**
 * Establece todos los leds apagados.
 * Limpia el buffer e invoca a funcion refresh()
 */
void LedsPanel::clearAll(void) {

  memset(ledsBuffer, 0x00, sizeof(ledsBuffer));

  refresh();

}


/**
 * Establece el valor para una seccion de leds:
 * FUNC_INDICATOR, BLUE, GREEN, WHITE, YELLOW,RED
 */
void LedsPanel::setValue(uint8_t ledsSection, uint8_t value) {
  setValue(ledsSection,value, 1);
}

void LedsPanel::setValue(uint8_t ledsSection, uint8_t value, byte doRefresh) {

  ledsBuffer[ledsSection] = value;

  if ( doRefresh )
    refresh();

}


/**
 * Establece el valor para una seccion de leds:
 * FUNC_INDICATOR, BLUE, GREEN, WHITE, YELLOW,RED
 * aplicando el operador OR de bits
 */
void LedsPanel::setValueOR(uint8_t ledsSection, uint8_t value) {

  setValueOR(ledsSection, value, 1);

}

void LedsPanel::setValueOR(uint8_t ledsSection, uint8_t value, byte doRefresh) {

  ledsBuffer[ledsSection] |= value;

  if ( doRefresh )
    refresh();

}


/**
 * Establece el valor para una seccion de leds:
 * FUNC_INDICATOR, BLUE, GREEN, WHITE, YELLOW,RED
 * aplicando el operador AND de bits
 */
void LedsPanel::setValueAND(uint8_t ledsSection, uint8_t value) {

  setValueAND(ledsSection, value, 1);

}

void LedsPanel::setValueAND(uint8_t ledsSection, uint8_t value, byte doRefresh) {

  ledsBuffer[ledsSection] &= value;

  if ( doRefresh )
    refresh();

}


/**
 * Establece el valor para las secciones correspondiente a la rueda:
 * BLUE, GREEN, WHITE, YELLOW,RED
 */
void LedsPanel::setWheelValues(uint8_t blue, uint8_t green, uint8_t white, uint8_t yellow, uint8_t red) {

  ledsBuffer[BLUE] = blue;
  ledsBuffer[GREEN] = green;
  ledsBuffer[WHITE] = white;
  ledsBuffer[YELLOW] = yellow;
  ledsBuffer[RED] = red;

  refresh();

}


void LedsPanel::setWheelValues(uint8_t ledNumber, byte value) {
  setWheelValues( ledNumber, value, 1);
}


void LedsPanel::setWheelValues(uint8_t ledNumber, byte value, byte doRefresh) {

  byte mask = 0x80;

  if ( ledNumber < 8 ) {
    mask >>= ledNumber;
    if(value)
      ledsBuffer[WHITE] |= mask;
    else
      ledsBuffer[WHITE] &= (~mask);
  }
  else if ( ledNumber < 16 ) {
    mask >>= (ledNumber - 8);
    if(value)
      ledsBuffer[GREEN] |= mask;
    else
      ledsBuffer[GREEN] &= (~mask);
  }
  else if ( ledNumber < 24 ) {
    mask >>= (ledNumber - 16);
    if(value)
      ledsBuffer[BLUE] |= mask;
    else
      ledsBuffer[BLUE] &= (~mask);
  }
  else if ( ledNumber < 32 ) {
    mask >>= (ledNumber - 24);
    if(value)
      ledsBuffer[RED] |= mask;
    else
      ledsBuffer[RED] &= (~mask);
  }
  else if ( ledNumber < 40 ) {
    mask >>= (ledNumber - 32);
    if(value)
      ledsBuffer[YELLOW] |= mask;
    else
      ledsBuffer[YELLOW] &= (~mask);
  }

  if ( doRefresh )
    refresh();

}


uint8_t LedsPanel::getWheelNValue(uint8_t ledNumber) {

  byte mask = 0x80;
  uint8_t retValue;

  if ( ledNumber < 8 ) {
    mask >>= ledNumber;
    retValue = ledsBuffer[WHITE] & mask;
  }
  else if ( ledNumber < 16 ) {
    mask >>= (ledNumber - 8);
    retValue = ledsBuffer[GREEN] & mask;
  }
  else if ( ledNumber < 24 ) {
    mask >>= (ledNumber - 16);
    retValue = ledsBuffer[BLUE] & mask;
  }
  else if ( ledNumber < 32 ) {
    mask >>= (ledNumber - 24);
    retValue = ledsBuffer[RED] & mask;
  }
  else if ( ledNumber < 40 ) {
    mask >>= (ledNumber - 32);
    retValue = ledsBuffer[YELLOW] & mask;
  }

  if ( retValue )
    retValue = 1;
  else
    retValue = 0;

  return retValue;

}


/**
 * Realiza una rotaci�n o desplazamiento en la rueda
 * principal de leds. Los argumetos determinan la direccion
 * (LEFT o RIGHT) y cantidad de posiciones desplazadas
 */

void LedsPanel::rotate(uint8_t direction) {
  rotate(direction, 1);
}

void LedsPanel::rotate(uint8_t direction, uint8_t steps) {
  rotate(direction, steps, 1);
}

void LedsPanel::rotate(uint8_t direction, uint8_t steps, byte doRefresh) {

  uint8_t bitSaved;

  switch(direction) {

    // Rotacion hacia la derecha
    case RIGHT: {

     /*
      * Resguarda el bit menos significativo de la
      * primera seccion (BLUE) de la rueda
      */
      bitSaved = ledsBuffer[BLUE] & B00000001;
      bitSaved <<= 7;

     /*
      * Realiza la rotacion a derecha
      * de todas las secciones de la rueda
      */
      for ( uint8_t i = BLUE ; i < RED ; i++ ) {
        ledsBuffer[i] >>= 1;
        ledsBuffer[i] &= B01111111;
        ledsBuffer[i] |= (ledsBuffer[i+1]<<7);
      }

     /*
      * Setea el valor resguardado en el bit mas significativo
      * de la ultima seccion (RED) de la rueda
      */
      ledsBuffer[RED] >>= 1;
      ledsBuffer[RED] &= B01111111;
      ledsBuffer[RED] |= bitSaved;

      break;
    }

    // Rotacion hacia la izquierda
    case LEFT: {

     /*
      * Resguarda el bit mas significativo de la
      * ultima seccion (RED) de la rueda
      */
      bitSaved = ledsBuffer[RED] & B10000000;
      bitSaved >>= 7;

     /*
      * Realiza la rotacion a izquierda
      * de todas las secciones de la rueda
      */
      for ( int i = RED ; i > BLUE ; i-- ) {
        ledsBuffer[i] <<= 1;
        ledsBuffer[i] &= B11111110;
        ledsBuffer[i] |= (ledsBuffer[i-1]>>7);
      }

     /*
      * Setea el valor resguardado en el bit menos significativo
      * de la primera seccion (BLUE) de la rueda
      */
      ledsBuffer[BLUE] <<= 1;
      ledsBuffer[BLUE] &= B11111110;
      ledsBuffer[BLUE] |= bitSaved;

      break;
    }

  }

  if ( doRefresh )
    refresh();

}


/**
 * Obtiene el valor de una seccion de leds determinada:
 * FUNC_INDICATOR, BLUE, GREEN, WHITE, YELLOW,RED
 */
uint8_t LedsPanel::getValue(uint8_t ledsSection) {

  return ledsBuffer[ledsSection];
}


/**
 * Obtiene el valor de todas las secciones de leds.
 * Devuelve el puntero al buffer
 */
uint8_t * LedsPanel::getValue(void) {

  return ledsBuffer;
}
