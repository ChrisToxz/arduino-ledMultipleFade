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
int fadeAmount = 1;  // Fade hoeveelheid
int brightness = 0; 

int bright[] ={0,0,0,0,0,0,0,0,0};

int step_size = 10; // leds per tree
int steps_size = 5;  // aantal steps

int loopi;
int t;
void setup()
{
  pinMode(2, OUTPUT); // indicator led
  FastLED.addLeds<WS2812B, PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,1500); 
  set_max_power_indicator_LED(2);
  Serial.begin(9600);

  // Turn all led offs
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void loop()
{ 
  for(int x=0; x<steps_size;x++){ // for elke tree
    if(bright[x] < 255){ // Alleen als ze nog niet op 100% zijn
      if(x == 0){ // Als het de eerste tree is (Geen check op waarde van vorige tree
         for (int i=0;i<step_size;i++){ // voor elke 10 leds op een tree
            step_array[x][i] = CHSV(255,0,bright[x]);
          }
         bright[x] = bright[x] + fadeAmount;
      }else{ // als het niet de eerste tree is
        int lastStep = x - 1; // var voor vorige tree nummer
        if(bright[lastStep] > 155 ){ // als vorige tree een waarde heeft hoger dan
          for (int i=0;i<step_size;i++){ // start dan deze tree met faden
            step_array[x][i] = CHSV(255,0,bright[x]);
          }
          bright[x] = bright[x] + fadeAmount;
        }
        
      }
      
    }
     
  }
       FastLED.show();





//#################  
//  if(t < 255){
//    t = t + 5;
//  }else{
//    t = 0;
//  }
//  loopIndicator(); // ledje op pcb knipper
//     for (int x=0; x<steps_size; x++){ // voor elke tree
//      for (int i=0;i<step_size;i++){ // voor elke 10 leds op een tree
//        
//        step_array[x][i] = CHSV(255,0,t);
//      }
//      FastLED.show();
//     }
// ###########  
//  Serial.println(brightness);
//    brightness = brightness + fadeAmount;
//  for (int x=0; x<steps_size; x++){ // voor elke tree
//    for (int i=0;i<step_size;i++){ // voor elke 10 leds op een tree
//      int y = x * 25; // verdubbel treenummer * 25
//      y = 0 - y; // 0 - bovenstaand
//      y = y + brightness; // een minus getal + hudiige brightness
//      if(y > 50 && y <= 255){ // alleen laten zien bij 50+ brightness, (anders rare kleurtjes op lage brightness ;s)
//      step_array[x][i] = CHSV(255,10,y); //x = tree i = led van die tree
//      FastLED.show(); // show
//       if(x==4&&y==255){
//      Serial.print("Brightness by reset");
//      Serial.println(brightness);
//      brightness = 0;
//    }
//      }
//    }
//   
//  }

  
}


void loopIndicator(){
      digitalWrite(2,HIGH);
      delay(5);
      digitalWrite(2,LOW);
}

