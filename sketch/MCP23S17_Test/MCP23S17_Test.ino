/*
  ESP32 SPI test using a 5V MCP23S17 GPIO expander through 3.3v - 5v level shifter
  Sets GPIO pins high and low to show results with LEDs connected to MCP23S17

  Gadget Reboot
*/

#include <Adafruit_MCP23X17.h>

#define CS_PIN 13
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18

#define OE_BUF_PIN 4   // Level shift buffer enable pin

Adafruit_MCP23X17 mcp;

void setup() {
  Serial.begin(115200);
  Serial.println("MCP23S17 Test");

  // enable level shift buffers
  pinMode(OE_BUF_PIN, OUTPUT);
  digitalWrite(OE_BUF_PIN, HIGH);

  // init MCP23S17 with specific SPI pins
  if (!mcp.begin_SPI(CS_PIN, SPI_SCK, SPI_MISO, SPI_MOSI)) {
    Serial.println("Fail to init.");
    while (1);
  }

  // set each GPIO expander output low and configure as outputs
  for (int i = 0; i < 15; i++) {
    mcp.digitalWrite(i, 0);
    mcp.pinMode(i, OUTPUT);
  }

  Serial.println("Running...");
}

void loop() {
  // turn LEDs on and off as a test
  for (int i = 0; i < 15; i++) {
    mcp.digitalWrite(i, 1);
    delay(200);
    mcp.digitalWrite(i, 0);
  }
}
