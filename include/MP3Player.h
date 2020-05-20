/*
 * MP3Player.h
 * Copyright 2019 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#ifndef MP3Player_h
#define MP3Player_h

#include "SoftwareSerial.h"
#include <DFRobotDFPlayerMini.h>


class MP3Player {

  // Nivel de volumen con valores de 0 a 30
  uint8_t volumeValue;

  // Instancia de objeto reproductor MP3
  DFRobotDFPlayerMini mp3Instance;

  // Puntero al puerto serie utilizado
  SoftwareSerial *mp3PlayerSerial;

public:

  /**
   * Constructor: inicializa el modo de los pines
   * RX y TX del puerto serie que se utilizara
   * para la comunicacion con el reproductor MP3
   */
  //MP3Player(int rxPin, int txPin);

  uint16_t getVolume(void);

  /**
   * Metodos que simplemente invocan a los
   * propios de la clase DFRobotDFPlayerMini
   * (se puede replanter el codigo utilizando herencia)
   */
  void begin(int rxPin, int txPin);
  void play(int track);
  void stop(void);
  void playFolder(uint8_t folderNumber, uint8_t fileNumber);
  void next(void);
  void previous(void);
  void volume(uint8_t value);
  void volumeDown(void);
  void volumeUp(void);
  uint16_t read(void);
  byte finished(void);
  uint8_t readType(void);
  void enableLoop(void);
  void disableLoop(void);

};

#endif
