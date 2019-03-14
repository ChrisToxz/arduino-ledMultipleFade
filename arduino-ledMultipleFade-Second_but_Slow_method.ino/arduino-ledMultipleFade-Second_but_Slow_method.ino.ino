#include "FastLED.h"
#define NUM_LEDS 50

// Settings
#define THRESHOLD_FADE 50 // Hoger = Volgende tree later starten. 0=alles direct
#define FADE_SPEED 10 // Lager = Sneller fade #delay in ms
#define FADE_AMOUNT 1 // Hoger = Snelle fade #brightness + fade_amount

//Trap
#define STEPS_SIZE 5 // aantal steps
#define STEP_SIZE 10 // leds per tree
// end of settings

// Setup leds
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
int bright[STEPS_SIZE];       // array om helderheid op te slaan per tree

// Vars
int lastFade = 0; // lastfade in ms. Voor delay

void setup()
{

  pinMode(2, OUTPUT); // indicator led
  FastLED.addLeds<WS2812B, PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,5000); 
  set_max_power_indicator_LED(2);
  Serial.begin(9600);

  // Turn all led offs
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void loop(){ 
if(delayy(lastFade,FADE_SPEED)){ // Delay without delaying function, zie onderaan de boolean functie (0,1)
  for(int x=0; x<STEPS_SIZE;x++){ // for elke tree
    if(bright[x] < 255){ // Alleen als ze nog niet op 100% zijn
      if(x == 0){ // Als het de eerste tree is (Geen check op waarde van vorige tree
         for (int i=0;i<STEP_SIZE;i++){ // voor elke 10 leds op een tree
            step_array[x][i] = CHSV(255,0,bright[x]);
          }
         bright[x] = bright[x] + FADE_AMOUNT;
      }else{ // als het niet de eerste tree is
        int lastStep = x - 1; // var voor vorige tree nummer
        if(bright[lastStep] > THRESHOLD_FADE ){ // als vorige tree een waarde heeft hoger dan
            for (int i=0;i<STEP_SIZE;i++){ // start dan deze tree met faden
              step_array[x][i] = CHSV(255,0,bright[x]);
            }
            bright[x] = bright[x] + FADE_AMOUNT;
        } 
      } 
    }
  }
  lastFade = millis();
}
       //FastLED.delay(1);
        FastLED.show();
}

boolean delayy(unsigned long since, unsigned long time) {
  unsigned long currentmillis = millis();
  if (currentmillis - since >= time) {
    since = currentmillis;
    return true;
  }
  return false;
}



