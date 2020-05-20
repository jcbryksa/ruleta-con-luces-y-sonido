/*
 * LedsPanel.h
 * Copyright 2019 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#ifndef LedsPanel_h
#define LedsPanel_h

#include <Arduino.h>

/*
 * Secciones dentro del
 * panel de LEDs
 */
#define FUNC_INDICATOR 0 // Columna de leds verdes indicadora de funciones
#define BLUE           1 // Tramo circular de leds azules
#define GREEN          2 //  "       "     "   "   verdes
#define WHITE          3 //  "       "     "   "   blancos
#define YELLOW         4 //  "       "     "   "   amarillos
#define RED            5 //  "       "     "   "   rojos

/*
 * Direcciones disponibles para
 * funcion de rotacion rotate()
 */
#define RIGHT          0
#define LEFT           1


class LedsPanel {

 /*
  * Configuracion de pines para
  * los shift registers CD4094
  */
  int enablePin;
  int clkPin;
  int dataPin;

 /*
  * Buffer de datos. Cada bit determina el estado
  * de encendido de un led. La posicion 0 del array
  * corresponde a la columna indicadora de funciones.
  * Desde la posicion 1 a la 5, cada uno de los tramos
  * circulares con colores azul, verde, blanco, amarillo y rojo
  */
  uint8_t ledsBuffer[6];


public:

  /**
   * Constructor: inicializa el modo de los pines (output)
   * el buffer en cero (todos leds apagados) e inhabilita
   * la linea de salida de todos los shift registers
   *
  LedsPanel(int penablePin, int pclkPin, int pdataPin);
  */
  void begin(int penablePin, int pclkPin, int pdataPin);

  /**
   * Establece todos los leds apagados.
   * Limpia el buffer e invoca a funcion refresh()
   */
  void clearAll(void);

  /**
   * Establece el estado encendido/apagado de
   * cada led en funcion de su estado logico
   * correspondiente en el buffer
   */
  void refresh(void);

  /**
   * Establece el valor para una seccion de leds:
   * FUNC_INDICATOR, BLUE, GREEN, WHITE, YELLOW,RED
   */
  void setValue(uint8_t ledsSection, uint8_t value);
  void setValue(uint8_t ledsSection, uint8_t value, byte doRefresh);

  /**
   * Establece el valor para una seccion de leds:
   * FUNC_INDICATOR, BLUE, GREEN, WHITE, YELLOW,RED
   * aplicando el operador OR de bits
   */
  void setValueOR(uint8_t ledsSection, uint8_t value);
  void setValueOR(uint8_t ledsSection, uint8_t value, byte doRefresh);

  /**
   * Establece el valor para una seccion de leds:
   * FUNC_INDICATOR, BLUE, GREEN, WHITE, YELLOW,RED
   * aplicando el operador AND de bits
   */
  void setValueAND(uint8_t ledsSection, uint8_t value);
  void setValueAND(uint8_t ledsSection, uint8_t value, byte doRefresh);

  /**
   * Establece el valor para las secciones correspondiente a la rueda:
   * BLUE, GREEN, WHITE, YELLOW,RED
   */
  void setWheelValues(uint8_t blue, uint8_t green, uint8_t white, uint8_t yellow, uint8_t red);
  void setWheelValues(uint8_t ledNumber, byte value);
  void setWheelValues(uint8_t ledNumber, byte value, byte doRefresh);

  /**
   * Obtiene el valor del led N de la rueda.
   */
  uint8_t getWheelNValue(uint8_t ledNumber);

  /**
   * Obtiene el valor de una seccion de leds determinada:
   * FUNC_INDICATOR, BLUE, GREEN, WHITE, YELLOW,RED
   */
  uint8_t getValue(uint8_t ledsSection);

  /**
   * Obtiene el valor de todas las secciones de leds.
   * Devuelve el puntero al buffer
   */
  uint8_t * getValue(void);

  /**
   * Realiza una rotación o desplazamiento en la rueda
   * principal de leds. Los argumetos determinan la direccion
   * (LEFT o RIGHT) y cantidad de posiciones desplazadas
   */
  void rotate(uint8_t direction);
  void rotate(uint8_t direction, uint8_t steps);
  void rotate(uint8_t direction, uint8_t steps, byte doRefresh);

};

#endif
