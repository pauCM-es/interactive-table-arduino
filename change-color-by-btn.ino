#include <FastLED.h>
#include <OneButton.h>

#define NUM_LEDS 18
#define NUM_LEDS_HEX 6
#define BTN1_PIN 3
#define BTN2_PIN 2
#define POT_PIN A1
#define DATA_PIN 13

#define a 5
#define b 6
#define c 7
#define d 8
#define e 9
#define f 10
#define g 11

CRGB leds[NUM_LEDS];

OneButton btn1 = OneButton(BTN1_PIN, false, true);
OneButton btn2 = OneButton(BTN2_PIN, false, true);
OneButton BTN_LIST[2] = {btn1, btn2};
int BTN_PIN_LIST[2] = {3, 2};
int segmentsDigit[7] = {a, b, c, d, e, f, g};
boolean digitList[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 0, 0, 1, 1}, // 9
};

int mode = 0;
int lightEffect = 0;

void setup()
{
  Serial.begin(57600);
  pinMode(BTN1_PIN, INPUT);
  pinMode(BTN2_PIN, INPUT);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);

  btn1.setClickMs(200);
  btn1.setPressMs(400);
  btn2.setPressMs(400);
  btn1.attachClick([]()
                   { changeLightEffect(&btn1); });
  btn1.attachDuringLongPress(lightHex, &btn1);
  btn1.attachLongPressStop(turnOffHex, &btn1);

  btn2.attachClick(changeLightEffect, &btn2);
  btn2.attachDuringLongPress(lightHex, &btn2);
  btn2.attachLongPressStop(turnOffHex, &btn2);

  for (int i = 0; i < 7; i++)
  {
    pinMode(segmentsDigit[i], OUTPUT);
  }
}

void loop()
{
  btn1.tick();
  btn2.tick();

  uint16_t potRead = analogRead(POT_PIN);
  uint8_t brightness = map(potRead, 0, 1023, 0, 200);
  FastLED.setBrightness(brightness);
  showDigit(lightEffect);
}

void showDigit(int digit)
{
  for (int i = 0; i < 7; i++)
  {
    boolean state = digitalRead(segmentsDigit[i]);
    boolean newState = digitList[digit][i];
    if (state != newState)
    {
      digitalWrite(segmentsDigit[i], newState);
    }
    delay(50);
  }
}

void cleanDigit()
{
  for (int i = 0; i < 7; i++)
  {
    digitalWrite(segmentsDigit[i], LOW);
  }
}

void lightHex(void *button)
{
  uint8_t sinBeat = beatsin8(30, 0, NUM_LEDS - 1, 0, 0);

  int btnPin = ((OneButton *)button)->pin();
  int hex = hexId(btnPin);
  int pixelStart = hex * 6;
  static uint8_t hue = 0;
  EVERY_N_MILLISECONDS(15)
  {
    hue++;
  }
  switch (lightEffect)
  {
  case 0:
    rotateHue(pixelStart);
    break;
  case 1:
    spinWheel(pixelStart);
    break;
  case 2:
    backAndFor(pixelStart);
    break;
  default:
    break;
  }
}

void turnOffHex(void *button)
{
  int btnPin = ((OneButton *)button)->pin();
  int hex = hexId(btnPin);
  int pixelStart = hex * 6;
  for (int i = pixelStart; i < pixelStart + NUM_LEDS_HEX; i++)
  {
    leds[i] = CRGB::Black;
    FastLED.show();
  }
}

int hexId(int btnPinHex)
{
  int hexId;
  for (int i = 0; i < sizeof(BTN_PIN_LIST); i++)
  {
    if (BTN_PIN_LIST[i] == btnPinHex)
    {
      hexId = i;
    }
  }
  return hexId;
}

void rotateHue(int pixelStart)
{
  static uint8_t hue = 0;
  EVERY_N_MILLISECONDS(15)
  {
    hue++;
  }
  for (int i = pixelStart; i < pixelStart + NUM_LEDS_HEX; i++)
  {
    leds[i] = CHSV(hue + (pixelStart * 10), 255, 255);
    FastLED.show();
  }
}

void backAndFor(int pixelStart)
{
  static uint8_t hue = 0;
  EVERY_N_MILLISECONDS(10)
  {
    hue++;
  }
  uint8_t sinBeat = beatsin8(60, pixelStart, pixelStart + 5, 0, pixelStart * 10);
  leds[sinBeat] = CHSV(hue, 255, 255);
  FastLED.show();
  fadeToBlackBy(leds, NUM_LEDS, 10);
}

void spinWheel(int pixelStart)
{
  static uint8_t hue = 0;
  EVERY_N_MILLISECONDS(2)
  {
    hue++;
  }
  for (int i = pixelStart; i < pixelStart + NUM_LEDS_HEX; i++)
  {
    leds[i] = CHSV(hue + (pixelStart * 10), 255, 255);
    FastLED.show();
    for (int j = i; j < pixelStart + NUM_LEDS_HEX; j++)
    {
      leds[j] = CRGB::Black;
      delay(6);
    }
    delay(6);
  }
}

static void changeLightEffect(void *button)
{
  Serial.println("effect changed!");
  int btnPin = ((OneButton *)button)->pin();
  // cleanDigit();
  if (btnPin == BTN1_PIN)
  {
    lightEffect = ++lightEffect % 3;
  }
  if (btnPin == BTN2_PIN)
  {
    if (lightEffect == 0)
    {
      lightEffect = 2;
    }
    else
    {
      lightEffect--;
    };
  }
  // showDigit(lightEffect);
  Serial.println(lightEffect);
}