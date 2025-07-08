/*
  ESP32 I2S MP3/WAV SD Card Playback Test
  Also performs an I2C bus scan to check for any devices connected (testing the level shifting buffers)

  Hardware:  Uses external I2S DAC (tested with PT8211)

  Uses ESP32-audioI2S Library https://github.com/schreibfaul1/ESP32-audioI2S
       Version 2.0.0 from mid 2023 was used successfully with
       Arduino IDE 1.8.13
       ESP32 board file 1.0.6

       This is one of those library experiences where using the latest ESP32 board support (mid 2025), library, and IDE
       just doesn't compile still after an hour of debugging the configuraiton, trying different combinations of
       libraries/rolling back board support, and the compile errors are down within the library so I just go with 
       what previously worked

  Gadget Reboot
*/

#include "Arduino.h"
#include <Wire.h>
#include "Audio.h"
#include "SD.h"
#include "FS.h"


// SD Card SPI pins
#define CS 5
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18

#define OE_BUF_PIN 4   // Level shift buffer enable pin

// I2S external DAC pins
#define I2S_DOUT 32  // data in to I2S module
#define I2S_LRC 25   // WS (word select)
#define I2S_BCLK 33  // bit clock

Audio audio;  // use external I2S DAC

void setup() {
  Serial.begin(115200);

  // set up I2C bus
  Wire.begin();
  Wire.setClock(400000);

  // enable level shift buffers
  pinMode(OE_BUF_PIN, OUTPUT);
  digitalWrite(OE_BUF_PIN, HIGH);

  i2c_scanner();  // check for I2C device addresses present on bus

  // configure SPI for SD card access
  // at this point the SD card chip select is held asserted for the demo
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

  // init SD card
  if (!SD.begin(CS)) {
    Serial.println("SD card error");
    while (true)
      ;
  }

  // configure external I2S DAC 
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setI2SCommFMT_LSB(true);  // data format for PT8211 is different from some other DACs 

  audio.setVolume(20);

  // play specific mp3 or wav file on SD card
  audio.connecttoFS(SD, "/testfile.mp3");
  //audio.connecttoFS(SD, "/testfile.wav");

}

void loop() {
  audio.loop();  // perform mp3 playback tasks
  vTaskDelay(1);
}

void i2c_scanner() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknow error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("done\n");
  }
  delay(500);
}
