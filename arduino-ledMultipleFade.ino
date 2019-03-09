#include "FastLED.h"
#define NUM_LEDS 50
CRGB rawleds[NUM_LEDS];
CRGBSet leds(rawleds, NUM_LEDS);
CRGBSet step1(leds(0,9));
CRGBSet step2(leds(10,19));
CRGBSet step3(leds(20,29));
CRGBSet step4(leds(30,39));
CRGBSet step5(leds(40,49));

struct CRGB * step_array[] ={step1,step2,step3,step4,step5};
#define PIN D5 
#define COLOR_ORDER GRB 
int fadeAmount = 5;  // Fade hoeveelheid
int brightness = 0; 

uint8_t step_size = 10; // leds per tree
uint8_t steps_size = 5;  // aantal steps

void setup()
{
  pinMode(2, OUTPUT); // indicator led
  FastLED.addLeds<WS2812B, PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(50);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,1500); 
  set_max_power_indicator_LED(13);
}

void loop()
{ 
  loopIndicator();
    brightness = brightness + fadeAmount;
  for (int x=0; x<steps_size; x++){
    for (int i=0;i<step_size;i++){
      int y = x * 25;
      y = 0 - y;
      y = y + brightness; 
      if(y > 0){
      step_array[x][i] = CHSV(255,10,y);
      }
          FastLED.show();
    }

  }
}


void loopIndicator(){
      digitalWrite(2,HIGH);
      delay(100);
      digitalWrite(2,LOW);
}

