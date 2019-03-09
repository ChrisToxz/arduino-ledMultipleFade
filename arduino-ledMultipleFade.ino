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

int step_size = 10; // leds per tree
int steps_size = 5;  // aantal steps

void setup()
{
  pinMode(2, OUTPUT); // indicator led
  FastLED.addLeds<WS2812B, PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(50);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,1500); 
  set_max_power_indicator_LED(2);
}

void loop()
{ 
  loopIndicator(); // ledje op pcb knipper
    brightness = brightness + fadeAmount;
  for (int x=0; x<steps_size; x++){ // voor elke tree
    for (int i=0;i<step_size;i++){ // voor elke 10 leds op een tree
      int y = x * 25; // verdubbel treenummer * 25
      y = 0 - y; // 0 - bovenstaand
      y = y + brightness; // een minus getal + hudiige brightness
      if(y > 50){ // alleen laten zien bij 50+ brightness, (anders rare kleurtjes op lage brightness ;s)
      step_array[x][i] = CHSV(255,10,y); //x = tree i = led van die tree
      FastLED.show(); // show
      }
    }

  }
}


void loopIndicator(){
      digitalWrite(2,HIGH);
      delay(100);
      digitalWrite(2,LOW);
}

