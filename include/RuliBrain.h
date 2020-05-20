/*
 * RuliBrain.h
 * Copyright 2019 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#ifndef RuliBrain_h
#define RuliBrain_h

#include "RotaryEncoder.h"
#include "MP3Player.h"
#include "LedsPanel.h"

/* Cantidad maxima de instancias de
 * intervalos que podran ser creadas
 * con el metodo getInterval
 */
#define INTERVALS    16

/*
 * Medida del buffer de datos de
 * proposito general utilizado por
 * las distintas funcionalidades
 */
#define DATA_SIZE    8


class RuliBrain {

  /*
   * Declaracion de punteros a
   * objetos principales de
   * la aplicacion
   */
  RotaryEncoder *mainWheel;
  RotaryEncoder *rotarySelector;
  MP3Player *mp3Player;
  LedsPanel *ledsPanel;

  // Eventos de los encoders
  uint8_t selectorEvent;
  uint8_t wheelEvent;

  // Funcionalidad que ejecuto previamente
  uint8_t prevFunction;

  // Funcionalidad actual en ejecucion
  uint8_t currentFunction;

  /* Funcionalidad seleccionada momentaneamente
   * durante la ejecucion del selector
   * de funcionalidades
   */
  uint8_t selectedFunction;

  /* Nivel de volumen establecido
   * para el reproductor MP3
   */
  uint8_t volume;

  /*
   * Indicador de paso o estado para uso
   * general en las funcionalidades
   */
  uint8_t currentStep;

  /*
   * Numero de sonido seleccionado para
   * indicar giro de la rueda principal
   */
  uint8_t spinSound;

 /*
  * Flags 1/0 que indican
  * distintos estados
  */
  byte initializeFunction;   // funcionalidad debe ser inicializada
  byte funcSelectorIsActive; // selector de funcionalidades esta en ejecucion
  byte volumeSettingIsActive; // ajuste de volumen esta en ejecucion
  byte speaking;  // ruli esta hablando
  byte spinning;  // la rueda se encuentra girando

 /*
  * Declaracion de vector para manejo de distintos
  * intervalos mediante el metodo getInterval
  */
  typedef struct {

    unsigned long timestamp;
    uint8_t currentStep;

  } Interval_t;
  Interval_t intervals[INTERVALS];

/*
 * Buffer de datos de proposito general utilizado
 * por las distintas funcionalidades
 */
  byte data[DATA_SIZE];


  /**
   * Devuelve un valor entre 0 y [steps] incrementando dicho valor
   * siempre y cuando el tiempo sea >= [ms] entre cada invocacion
   * se devera especificar un indicador de intervalo [interval] que
   * puede ser de 0 a INTERVALS
   *
   */
  uint8_t getInterval(uint8_t interval, unsigned long ms, uint8_t steps);

  void resetInterval(uint8_t interval);

  void mp3FinishFlush(void);

  void functionSelector(void);

  void volumeSetting(void);

  void speak(uint8_t folderNumber, uint8_t fileNumber);

  void ledSpeakEffect(void);

  void playSpinSound(void);

  void iddleCheck(void);

 //
 // Funcionalidades de Ruli
 //
  void welcome(void);
  void simpleRoulette(void);
  void randomColor(void);
  void followTheColor(void);
  void turnMeter(void);
  void velocityMeter(void);
  void customShape(void);
  void soundShooting(void);
  void music(void);
 ///


public:

  /**
   * Metodo de inicializacion
   * simil constructor
   */
  void begin(RotaryEncoder *pmainWheel, RotaryEncoder *protarySelector, MP3Player *pmp3Player, LedsPanel *pledsPanel);

  // Metodo de ejecucion principal
  void run(void);

};

#endif
