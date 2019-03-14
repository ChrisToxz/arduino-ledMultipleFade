#include "FastLED.h"
#define NUM_LEDS 390
#define WC_LEDS 195

// Settingss

//PINS
#define PIN 51 // debug
#define TRAP_PIN 51 // LED Data
#define BOVEN_PIN 50 // PIR Boven
#define BENEDEN_PIN 49 // PIR Beneden
#define WCPIR_PIN 52 // PIR WC
#define WC_PIN 53 // WC LED Data


/* FADE SETTING */

// WC
#define WC_FADE 2

// Trap
#define THRESHOLD_FADE 50                        // Hoger = Volgende tree later starten. 0=alles direct
#define FADE_SPEED 15                            // Lager = Sneller fade #delay in ms
#define FADE_AMOUNT 5                            // Hoger = Snelle fade #brightness + fade_amount

//Trap
#define STEPS_SIZE 13                            // aantal steps
#define STEP_SIZE 30                             // leds per tree
/* END OF SETTINGS */

// Setup leds
CRGB rawleds[NUM_LEDS];
CRGB wc_raw_leds[WC_LEDS];
CRGBSet wcleds(wc_raw_leds,WC_LEDS);
CRGBSet leds(rawleds, NUM_LEDS);
CRGBSet step1(leds(0,29));                       // tree 1 heeft led 0 tot en met 9
CRGBSet step2(leds(30,59));                     // tree 2 heeft led 10 tot en met 19
CRGBSet step3(leds(60,89));// etc etc etc
CRGBSet step4(leds(90,119));
CRGBSet step5(leds(120,149));
CRGBSet step6(leds(150,179));
CRGBSet step7(leds(180,209));
CRGBSet step8(leds(210,239));
CRGBSet step9(leds(240,269));
CRGBSet step10(leds(270,299));
CRGBSet step11(leds(300,329));
CRGBSet step12(leds(330,359));
CRGBSet step13(leds(360,389));
struct CRGB * step_array[] ={step1,step2,step3,step4,step5,step6,step7,step8,step9,step10,step11,step12,step13}; // Google <3

#define COLOR_ORDER GRB 
int bright[STEPS_SIZE];       // array om helderheid op te slaan per tree

/* VARS */
int bright_wc = 0;
int lastFade = 0; // lastfade in ms. Voor delay
int up,down;
int wc;
int wc_time;
int upFade;


void setup()
{
  // Setup pins
  pinMode(2, OUTPUT); // indicator led
  pinMode(WCPIR_PIN, INPUT); // WC PIR
  pinMode(BOVEN_PIN, INPUT); // Boven PIR
  pinMode(BENEDEN_PIN, INPUT); // Beneden PIR
  FastLED.addLeds<WS2812B, PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.addLeds<WS2812B, WC_PIN, COLOR_ORDER>(wcleds, WC_LEDS);
  FastLED.setBrightness(50);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,5000); 
  set_max_power_indicator_LED(13l);
  Serial.begin(9600);

  // Turn all led offs
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  fill_solid(wcleds, WC_LEDS, CRGB::Black);
  FastLED.show();
}

void loop(){ 

  /* WC CODE */
  // Need some touch up ;)

  // Fade up
  Serial.println(wc);
  if(digitalRead(WCPIR_PIN) == HIGH && wc == 0){
    wc = 1;
    if(bright_wc == 0){
      for (int i=bright_wc;i<255;i++){ //TODO: Fade sneller maken maar bij +2 komt helderheid op 256 en dat resulteert in 0% helderheid.
      bright_wc = bright_wc + 1;
      for (int x=0;x<WC_LEDS;x++){               // voor elke 10 leds op een tree
            wcleds[x] = CHSV(255,0,bright_wc); // zet HSV
          }
        FastLED.show();
    }
    wc_time = millis();
    }
  }

  // Fade down after minute
  if(delayy(wc_time, 60000) && wc == 1){
    fill_solid(wcleds, WC_LEDS, CRGB::Black);
    FastLED.show();
    wc = 0;
    bright_wc = 0;
  }

checkPir();
// Beneden sensor trigger
if(up == 1 && down == 0){
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
    }else{
      if(bright[STEPS_SIZE - 1] == 255){
        up = 2; 
      }
    }
  }
  lastFade = millis();                                // Sla millis op van laatste fade voor de delay
}
       //FastLED.delay(1); // Andere functie om de fade te vertragen, 1 vetraagt al gigantisch
        FastLED.show();   // Show led 
        upFade = millis();
}
//Serial.println(up);

if(delayy(upFade,5000) && up == 2){
  up = 3;
}

if(up == 3){                      
  for(int x=0; x<STEPS_SIZE;x++){                     // for elke tree
    if(bright[x] > 0){                              // Alleen verder indien bright van die tree lager is dan 255(=max brightness)
      if(x == 0){                                     // Indien het de eerste tree is (Kan geen check uitvoeren of de vorige led een waarde heeft, zie code hier onder)
         for (int i=0;i<STEP_SIZE;i++){               // voor elke 10 leds op een tree
            step_array[x][i] = CHSV(255,0,bright[x]); // zet HSV
          }
         bright[x] = bright[x] - FADE_AMOUNT;         // set bright voor volgende run op brightness + fade amount
      }else{                                          // als het niet de eerste tree is (Dus kan de waarde van de vorige tree checken)
        int lastStep = x - 1;                         // var voor vorige tree id
        if(bright[lastStep] < THRESHOLD_FADE ){       // als vorige tree een waarde heeft hoger dan de THRESHOLD (Pas gaan starten als de vorige tree op bijv 50 brightness zit)
            for (int i=0;i<STEP_SIZE;i++){            // Voor elke 10 leds op een tree
              step_array[x][i] = CHSV(255,0,bright[x]);// set HSV
            }
            bright[x] = bright[x] - FADE_AMOUNT;      // set bright voor volgende run op brightness + fade amount
        } 
      } 
    }else{
      if(bright[STEPS_SIZE - 1] == 0){
        up = 0; 
      }
    }
  }
       //FastLED.delay(1); // Andere functie om de fade te vertragen, 1 vetraagt al gigantisch
        FastLED.show();   // Show led 
}

if(down == 1 && up == 0){
 if(delayy(lastFade,FADE_SPEED)){                      // Delay without delaying function, zie onderaan de boolean functie (0,1)
  for(int x=STEPS_SIZE; x-- > 0;){                     // for elke tree
    if(bright[x] < 255){                              // Alleen verder indien bright van die tree lager is dan 255(=max brightness)
      if(x == STEPS_SIZE){                                     // Indien het de eerste tree is (Kan geen check uitvoeren of de vorige led een waarde heeft, zie code hier onder)
         for (int i=0;i<STEP_SIZE;i++){               // voor elke 10 leds op een tree
            step_array[x][i] = CHSV(255,0,bright[x]); // zet HSV
          }
         bright[x] = bright[x] + FADE_AMOUNT;         // set bright voor volgende run op brightness + fade amount
      }else{                                          // als het niet de eerste tree is (Dus kan de waarde van de vorige tree checken)
        int lastStep = x + 1;                         // var voor vorige tree id
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
}

boolean delayy(unsigned long since, unsigned long time) {
  unsigned long currentmillis = millis();
  if (currentmillis - since >= time) {
    since = currentmillis;
    return true;
  }
  return false;
}

boolean checkPir(){

  if(digitalRead(BENEDEN_PIN) == HIGH && up == 0){
    up = 1;
  }
  if(digitalRead(BOVEN_PIN) == HIGH && down == 0){
    down = 1;
  }
  
}




