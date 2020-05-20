/*
 * MP3Player.cpp
 * Copyright 2019 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#include "SoftwareSerial.h"

/*
 * Header de la biblioteca DFRobotDFPlayerMini
 * que debe ser instalada previamente
 */
#include <DFRobotDFPlayerMini.h>

#include "MP3Player.h"

/**
 * Inicializa el modo de los pines
 * RX y TX del puerto serie que se utilizara
 * para la comunicacion con el reproductor MP3
 */
void MP3Player::begin(int rxPin, int txPin) {

 /*
  * Inicializacion del reproductor MP3 con comunicacion
  * a traves del puerto serie establecido
  */
  mp3PlayerSerial = new SoftwareSerial(rxPin, txPin);
  mp3PlayerSerial->begin(9600);
  mp3Instance.begin(*mp3PlayerSerial);

  volumeValue = 3;
  mp3Instance.volume(volumeValue);  //Set volume value. From 0 to 30

}


byte MP3Player::finished(void) {

  byte vret = 0;

  if ( mp3Instance.available() )
    if ( mp3Instance.readType() == DFPlayerPlayFinished )
      vret = 1;

  return vret;

}


uint8_t MP3Player::readType(void) {

  uint8_t vret = 0;

  if ( mp3Instance.available() )
    vret = mp3Instance.readType();

  return vret;

}



/**
 * Metodos que simplemente invocan a los
 * propios de la clase DFRobotDFPlayerMini
 * (se puede replantear el codigo utilizando herencia)
 */
/*** BEGIN ***/
void MP3Player::play(int track) {
  mp3Instance.play(track);
}

void MP3Player::stop(void) {
  mp3Instance.stop();
}

void MP3Player::playFolder(uint8_t folderNumber, uint8_t fileNumber) {
  mp3Instance.playFolder(folderNumber, fileNumber);
}

void MP3Player::next(void) {
  mp3Instance.next();
}

void MP3Player::previous(void) {
  mp3Instance.previous();
}

void MP3Player::volume(uint8_t value) {
  volumeValue = value;
  mp3Instance.volume(value);
}

void MP3Player::volumeDown(void) {
  if ( volumeValue > 2 ) {
    volumeValue--;
    mp3Instance.volumeDown();
  }
}

void MP3Player::volumeUp(void) {
  if ( volumeValue < 30 ) {
    volumeValue++;
    mp3Instance.volumeUp();
  }
}

uint16_t MP3Player::getVolume(void) {
  return volumeValue;
}

uint16_t MP3Player::read(void) {
  return mp3Instance.read();
}

void MP3Player::enableLoop(void) {
  mp3Instance.enableLoop();
}

void MP3Player::disableLoop(void) {
  mp3Instance.disableLoop();
}

/*** END ***/
