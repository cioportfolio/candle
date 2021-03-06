#include <FastLED.h>

uint8_t gamma8[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
    2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
    5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
    10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
    17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
    25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
    37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
    51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
    69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
    90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
    115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
    144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
    177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255};

#define LED_PIN 5
//#define SW_PIN 2
//#define X_PIN 0
//#define Y_PIN 1

#define COLOR_ORDER GRB
#define CHIPSET WS2812B

#define BRIGHTNESS 128
#define UPDATES_PER_SECOND 40

#define HEIGHT 9
#define NUM_LEDS (HEIGHT * 3 + 1)
CRGB leds[NUM_LEDS];

uint8_t tbuff[2][HEIGHT + 1];
uint8_t temps = 0;
uint8_t newt = 1;
uint8_t hue = 25;
uint8_t hue_step = 1;
uint8_t brightness = BRIGHTNESS;

CRGB correct(CRGB col)
{
  //  return CRGB(gamma8[col.r],gamma8[col.g],gamma8[col.b]);
  return col;
}

CRGB color(uint8_t temp)
{
  if (temp < 86)
  {
    return CHSV(hue, 255, temp * 192 / 86);
  }
  if (temp < 172)
  {
    return CHSV(hue, 255 - (temp - 86) * 128 / 86, 192 + (temp - 86) * 64 / 86);
  }
  return CHSV(hue, 128 - (temp - 172) * 128 / 86, 255);
}

void genDisplay()
{
  for (uint8_t y = 0; y < HEIGHT; y++)
  {
      CRGB col = correct(color(tbuff[temps][y]));
      leds[HEIGHT-1-y]=col;
      leds[HEIGHT+y]=col;
      leds[3*HEIGHT-y]=col;
  }
}

uint8_t safeTemp(uint8_t y)
{
  if (y < HEIGHT + 1)
  {
    return tbuff[temps][y];
  }
  return 0;
}

void updateTs()
{
  if (random8() > 75)
    tbuff[temps][HEIGHT] = random8(120, 215);

  tbuff[newt][HEIGHT] = 0;

  for (uint8_t y = 0; y < HEIGHT; y++)
  {
      tbuff[newt][y] = scale8(tbuff[temps][y + 1], 160);
      for (uint8_t row = 0; row < 3; row++)
      {
          tbuff[newt][y] += scale8(safeTemp(y + row) >> 2, random8(40, 60));
      }
  }
  temps = newt;
  newt = 1 - temps;
}

void Refresh()
{
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void loop()
{
  //uint16_t joyX, joyY;

  random16_add_entropy(random());

  unsigned long ms = millis();
  if (ms < 5000)
  {
    FastLED.setBrightness(scale8(brightness, (ms * 256) / 5000));
  }
  else
  {
    FastLED.setBrightness(brightness);
  }

  // joyX = analogRead(X_PIN);
  // joyY = analogRead(Y_PIN);
  // if (digitalRead(SW_PIN)==0) {
  //   hue_step = 1 - hue_step;
  // }
  // brightness += (joyY >> 8)-2;
  FastLED.setBrightness(brightness);
  // hue += (joyX >> 8)-2;
  // hue += hue_step;
  // Serial.print("Joy Y: ");
  // Serial.print(joyY);
  // Serial.print(" Hue: ");
  // Serial.print(hue);
  // Serial.print("\n\n");
  updateTs();
  genDisplay();
  Refresh();
}

void setup()
{
  delay(3000); // power-up safety delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  // pinMode(SW_PIN, INPUT);
  // digitalWrite(SW_PIN, HIGH);
  // Serial.begin(9600);
}
