#include <FastLED.h>
#include <OneButton.h>
#include "DigitModule.h"

#define NUM_LEDS 18
#define NUM_LEDS_HEX 6
#define POT_PIN A1
#define DATA_PIN 13

CRGB leds[NUM_LEDS];

int BTN_PIN_LIST[2] = { 3, 2 };
OneButton BTN_LIST[2];
int segmentsDigit[7] = { 5, 6, 7, 8, 9, 10, 11 };
DigitModule digit = DigitModule(segmentsDigit);

int mode = 0;
int lightEffect = 0;
int isRunning = false;

void setup() {
  Serial.begin(57600);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
long BTN_NAMES[2] = {"btn1", "btn2"};

  for (int i = 0; i < sizeof(BTN_PIN_LIST); i++) {
    pinMode(BTN_PIN_LIST[i], INPUT);
    BTN_LIST[i] = OneButton(BTN_PIN_LIST[0], false, true);
    OneButton BTN_NAMES[i] = BTN_LIST[i];
  }

  for (int i = 0; i < 7; i++) {
    pinMode(segmentsDigit[i], OUTPUT);
  }
}

void loop() {
  for (int i = 0; i < sizeof(BTN_LIST); i++) {
    BTN_LIST[i].tick();
  }

  uint16_t potRead = analogRead(POT_PIN);
  uint8_t brightness = map(potRead, 0, 1023, 0, 200);
  FastLED.setBrightness(brightness);
  digit.show(lightEffect);
}

int getHexId(int btnPinHex) {
  int hexId;
  for (int i = 0; i < sizeof(BTN_PIN_LIST); i++) {
    if (BTN_PIN_LIST[i] == btnPinHex) {
      hexId = i;
    }
  }
  return hexId;
}


static void changeLightEffect() {
  Serial.println("effect changed!");
  // int btnPin = ((OneButton *)button)->pin();
  digit.clean();
  lightEffect = ++lightEffect % 3;
  digit.show(lightEffect);
  Serial.println(lightEffect);
}

//HEX MODULES
void lightHex(void *button) {
  uint8_t sinBeat = beatsin8(30, 0, NUM_LEDS - 1, 0, 0);

  int btnPin = ((OneButton *)button)->pin();
  int hex = getHexId(btnPin);
  int pixelStart = hex * NUM_LEDS_HEX;
  static uint8_t hue = 0;
  EVERY_N_MILLISECONDS(15) {
    hue++;
  }
  switch (lightEffect) {
    case 0:
      rotateHue(pixelStart);
      break;
    case 1:
      spinWheel(pixelStart);
      break;
    case 2:
      backAndFor(pixelStart);
      break;
  }
}

void turnOffHex(void *button) {
  int btnPin = ((OneButton *)button)->pin();
  int hex = getHexId(btnPin);
  int pixelStart = hex * NUM_LEDS_HEX;
  for (int i = pixelStart; i < pixelStart + NUM_LEDS_HEX; i++) {
    leds[i] = CRGB::Black;
    FastLED.show();
  }
}

//LIGHT EFFECTS
void rotateHue(int pixelStart) {
  static uint8_t hue = 0;
  EVERY_N_MILLISECONDS(15) {
    hue++;
  }
  for (int i = pixelStart; i < pixelStart + NUM_LEDS_HEX; i++) {
    leds[i] = CHSV(hue + (pixelStart * 10), 255, 255);
    FastLED.show();
  }
}

void backAndFor(int pixelStart) {
  static uint8_t hue = 0;
  EVERY_N_MILLISECONDS(10) {
    hue++;
  }
  uint8_t sinBeat = beatsin8(60, pixelStart, pixelStart + 5, 0, pixelStart * 10);
  leds[sinBeat] = CHSV(hue, 255, 255);
  FastLED.show();
  fadeToBlackBy(leds, NUM_LEDS, 10);
}

void spinWheel(int pixelStart) {
  static uint8_t hue = 0;
  EVERY_N_MILLISECONDS(2) {
    hue++;
  }
  for (int i = pixelStart; i < pixelStart + NUM_LEDS_HEX; i++) {
    leds[i] = CHSV(hue + (pixelStart * 10), 255, 255);
    FastLED.show();
    for (int j = i; j < pixelStart + NUM_LEDS_HEX; j++) {
      leds[j] = CRGB::Black;
      delay(6);
    }
    delay(6);
  }
}