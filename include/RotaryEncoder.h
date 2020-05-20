/*
 * RotaryEncoder.h
 * Copyright 2019 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#ifndef RotaryEncoder_h
#define RotaryEncoder_h

/*
 * Acciones disponibles para
 * el encoder rotativo
 */
#define NONE            0
#define LEFT_TURN       1 // Giro hacia la izquierda (antihorario)
#define RIGHT_TURN      2 //  "     "    " derecha (horario)
#define SWITCH_CLICK    3 // Click en el pulsador
#define SWITCH_HELD     4 // Retencion del pulsador

class RotaryEncoder {

  // Ultimo nivel logico leido del pin CLK
  uint8_t lastClkPinLevel;

  // Pin "CLK"
  uint8_t clkPin;

  // Pin "DATA"
  uint8_t dataPin;

  // Pin "SWITCH"
  uint8_t switchPin;

  unsigned long switchTimestamp;
  uint8_t savedEvent;

public:

  /**
  * Constructor: inicializa el modo de los pines
  * correspondientes al encoder: CLK, DATA y SWITCH
  */
  //RotaryEncoder(uint8_t pclkPin, uint8_t pdataPin, uint8_t pswitchPin);
  //RotaryEncoder(uint8_t pclkPin, uint8_t pdataPin);
  void begin(uint8_t pclkPin, uint8_t pdataPin, uint8_t pswitchPin);
  void begin(uint8_t pclkPin, uint8_t pdataPin);

  /**
  * Obtiene la accion o evento
  * producido en el encoder
  */
  uint8_t getEvent(void);

};

#endif
