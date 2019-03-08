#include <FastLED.h>
#define NUM_LEDS 50
#define PIN D5
CRGB leds[NUM_LEDS];

void setup() { 
       FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUM_LEDS);
       FastLED.setBrightness(75);
}

void loop() { 
  fill_solid(leds, NUM_LEDS, CHSV(85, 255, 255));
  FastLED.show();
}
