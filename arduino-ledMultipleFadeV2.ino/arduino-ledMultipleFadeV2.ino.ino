#include "FastLED.h"
#define NUM_LEDS 50

// Settings
#define THRESHOLD_FADE 50                        // Hoger = Volgende tree later starten. 0=alles direct
#define FADE_SPEED 20                            // Lager = Sneller fade #delay in ms
#define FADE_AMOUNT 1                            // Hoger = Snelle fade #brightness + fade_amount
// Als je het op 50,20,1 zet is het effect duidelijk te zien

//Trap
#define STEPS_SIZE 5                             // aantal steps
#define STEP_SIZE 10                             // leds per tree
// end of settings

// Setup leds
CRGB rawleds[NUM_LEDS];
CRGBSet leds(rawleds, NUM_LEDS);
CRGBSet step1(leds(0,9));                       // tree 1 heeft led 0 tot en met 9
CRGBSet step2(leds(10,19));                     // tree 2 heeft led 10 tot en met 19
CRGBSet step3(leds(20,29));// etc etc etc
CRGBSet step4(leds(30,39));
CRGBSet step5(leds(40,49));
struct CRGB * step_array[] ={step1,step2,step3,step4,step5}; // Google <3
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
if(delayy(lastFade,FADE_SPEED)){                      // Delay without delaying function, zie onderaan de boolean functie (0,1)
  for(int x=0; x<STEPS_SIZE;x++){                     // for elke tree
    if(bright[x] < 255){                              // Alleen verder indien bright van die tree lager is dan 255(=max brightness)
      if(x == 0){                                     // Indien het de eerste tree is (Kan geen check uitvoeren of de vorige led een waarde heeft, zie code hier onder)
         for (int i=0;i<STEP_SIZE;i++){               // voor elke 10 leds op een tree
            step_array[x][i] = CHSV(255,0,bright[x]); // zet HSV
          }
         bright[x] = bright[x] + FADE_AMOUNT;         // set bright voor volgende run op brightness + fade amount
      }else{                                          // als het niet de eerste tree is (Dus kan de waarde van de vorige tree checken)
        int lastStep = x - 1;                         // var voor vorige tree id
        if(bright[lastStep] > THRESHOLD_FADE ){       // als vorige tree een waarde heeft hoger dan de THRESHOLD (Pas gaan starten als de vorige tree op bijv 50 brightness zit)
            for (int i=0;i<STEP_SIZE;i++){            // Voor elke 10 leds op een tree
              step_array[x][i] = CHSV(255,0,bright[x]);// set HSV
            }
            bright[x] = bright[x] + FADE_AMOUNT;      // set bright voor volgende run op brightness + fade amount
        } 
      } 
    }
  }
  lastFade = millis();                                // Sla millis op van laatste fade voor de delay
}
       //FastLED.delay(1); // Andere functie om de fade te vertragen, 1 vetraagt al gigantisch
        FastLED.show();   // Show led
}

boolean delayy(unsigned long since, unsigned long time) {
  unsigned long currentmillis = millis();
  if (currentmillis - since >= time) {
    since = currentmillis;
    return true;
  }
  return false;
}



