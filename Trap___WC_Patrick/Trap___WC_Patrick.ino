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
#define WC_FADE 1

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
long lastFade = 0; // lastfade in ms. Voor delay
int up,down; // weg halen
int wc;
int wc_mode;
int wc_direction;
long wc_time;
long upFade;


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
Serial.println(wc_mode);
checkPir();

/* WC CODE */

// pir check
if(digitalRead(WCPIR_PIN) == HIGH){ 
  if(wc == 0){
    Serial.println("WC Getriggered, wc was 0");
     wc = 1;
     for(bright_wc = 0; bright_wc < 210; bright_wc = bright_wc + 3){
      for (int x=0;x<WC_LEDS;x++){               // voor elke 10 leds op een tree
            wcleds[x] = CHSV(255,0,bright_wc); // zet HSV
          }
        FastLED.show();
     }
      wc_time = millis();
     Serial.println("Fade klaar");
   wc = 2;
  }else if(wc == 2){
    Serial.println("Extra tijd getriggerd");
    wc_time = millis();
  }
}

if(delayy(wc_time,15000) && wc == 2){
  Serial.println("Timer over, fade out");
    fill_solid(wcleds, WC_LEDS, CRGB::Black);
    FastLED.show();
    bright_wc = 0;
    wc = 0;
}else{
  Serial.println("Nog geen tijd of wc niet aan");
}

  /* TRAP SECTION */
switch(wc_mode){
  case 1:
  // Fade
    if(wc_direction == 0){ // if fade naar boven moet
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
              // Fade naar boven done?
              wc_mode = 2;
            }
          }
        }
        lastFade = millis();                                // Sla millis op van laatste fade voor de delay
      }
    //FastLED.delay(1); // Andere functie om de fade te vertragen, 1 vetraagt al gigantisch. Nu delay via millis, zie regel 118
    FastLED.show();   // Show led 
    upFade = millis(); // ??
  }else if(wc_direction == 1){ // fade van boven naar beneden
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
      }else{
        if(bright[0] == 255){
          //fade naar beneden done?
          wc_mode = 2;
        }
      }
    }
    lastFade = millis();                                // Sla millis op van laatste fade voor de delay
    }
      //FastLED.delay(1); // Andere functie om de fade te vertragen, 1 vetraagt al gigantisch
       FastLED.show();   // Show led 
       upFade = millis();
  }else{
    // error
  }
  
  break;

  case 2:
  // Fades klaar, check pir en verleng timeout.

  if(delayy(upFade,10000) && wc_mode == 2){
    wc_mode = 3; // fade out
  }else{
    if(digitalRead(BENEDEN_PIN) == HIGH || digitalRead(BOVEN_PIN) == HIGH){
      // als beweging is terwijl trap aan is
      upFade = millis(); // Reset timer
    }
  }
  break;
  case 3:
  // fade uit
  // check pir, if so reset fade go to mode 4
  if(digitalRead(BENEDEN_PIN) == HIGH || digitalRead(BOVEN_PIN) == HIGH){
    //wc_mode = 4;
  }
  if(wc_direction == 0){ // beneden uit richting boven
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
          wc_mode = 0;
        }
      }
    }
       //FastLED.delay(1); // Andere functie om de fade te vertragen, 1 vetraagt al gigantisch, nu delay via millis
        FastLED.show();   // Show led 
        
  }else if(wc_direction == 1){ // fade van boven naar benden uit
    for(int x=STEPS_SIZE; x-- > 0;){                     // for elke tree
        if(bright[x] > 0){                              // Alleen verder indien bright van die tree lager is dan 255(=max brightness)
          if(x == STEPS_SIZE - 1){                                     // Indien het de eerste tree is (Kan geen check uitvoeren of de vorige led een waarde heeft, zie code hier onder)
            for (int i=0;i<STEP_SIZE;i++){               // voor elke 10 leds op een tree
                step_array[x][i] = CHSV(255,0,bright[x]); // zet HSV
              }
            bright[x] = bright[x] - FADE_AMOUNT;         // set bright voor volgende run op brightness + fade amount
          }else{                                          // als het niet de eerste tree is (Dus kan de waarde van de vorige tree checken)
            int lastStep = x + 1;                         // var voor vorige tree id
            if(bright[lastStep] < THRESHOLD_FADE ){       // als vorige tree een waarde heeft hoger dan de THRESHOLD (Pas gaan starten als de vorige tree op bijv 50 brightness zit)
                for (int i=0;i<STEP_SIZE;i++){            // Voor elke 10 leds op een tree
                  step_array[x][i] = CHSV(255,0,bright[x]);// set HSV
                }
                bright[x] = bright[x] - FADE_AMOUNT;      // set bright voor volgende run op brightness + fade amount
            } 
          } 
        }else{
          if(bright[0] == 0){
            wc_mode = 0;
          }
        }
      }
       //FastLED.delay(1); // Andere functie om de fade te vertragen, 1 vetraagt al gigantisch, nu delay via millis
        FastLED.show();   // Show led 
  }else{
    // error
  }
  break;
  case 4:
  // Restore fade out.
  break;
  default:
  //ereror
  break;
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

  if(digitalRead(BENEDEN_PIN) == HIGH && wc_mode == 0){
    wc_mode = 1;
    wc_direction = 0; // from down to up
    //up = 1;
  }
  if(digitalRead(BOVEN_PIN) == HIGH && wc_mode == 0){
    wc_mode = 1;
    wc_direction = 1; // from up to down
    //down = 1;
  }


//
//  if(digitalRead(WCPIR_PIN) == LOW && wc == 2){
//    wc = 3;
//  }
}

// WC Modes
// 0 = UIT
// 1 = fade
// 2 = fade klaar
// 3 = fade uit
