

/*
Pimp my quechua lamp V2.
Rotary encoder with pushbutton to select mode + cycle in each mode.
Mode 1 : ON white, increase/decrease brightness.
Mode 2: Color, switch color from rainbow.
Mode 3: Cycle rainbow, increase/decrease speed.
Mode 4: OFF

Decathlon Quechua camping lamp, guts removed, reuse battery case.
Attiny 85:
Neopixel:
Rotary encoder with push button.
Flo Gales 2020. Licence rien à branler. Do what the fuck you want license.

*/
#include <SimpleRotary.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NBPIXEL 12
#define PIXEL_PIN 1
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NBPIXEL, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Rotary encode, A pin, B pin, switch pin.
#define APIN 3
#define BPIN 4
#define BTNPIN 2
SimpleRotary rotary(APIN,BPIN,BTNPIN);

// lamp mode
byte mode = 0; //off
byte tor = 0;
byte brightness = 10;
int tempo = 100;
byte wheelVal = 0;
byte cycleWheelVal = 0;
unsigned long lastMillis = 0;

void setup() {
  // For attiny85.
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  strip.begin();
  strip.clear();
  strip.show(); 
  strip.setBrightness(brightness); // Min brighness at startup.
  //TODO: backup brightness and speed.
}

void loop() {

  unsigned long currentMillis = millis();
  // change mode.
  if ( rotary.push() == 1 ) {
    mode = mode + 1 > 3 ? 0 : mode + 1;
    if(mode == 0){ //off
      strip.clear();
      strip.show(); 
    } 
  }

  
  // 0 = not turning, 1 = CW, 2 = CCW
  byte rot = rotary.rotate();

  // CW rotate.
  if (rot == 1 ) {
    if(mode == 1){
      brightness = brightness + 5 > 255 ? 255 : brightness + 5;
    } else if(mode == 2){
      wheelVal = wheelVal + 2 > 255 ? 255 : wheelVal + 2;
    } else if(mode == 3){
      tempo = tempo - 5 < 5 ? 5 : tempo - 5;
    }
  } else  if (rot == 2 ) { // CCW rotate
    if(mode == 1){
      brightness = brightness - 5 < 10 ? 10 : brightness - 5;
    } else if(mode == 2){
      wheelVal = wheelVal - 2 < 0 ? 0 : wheelVal - 2;
    } else if(mode == 3){
      tempo = tempo + 5 > 500 ? 500 : tempo + 5;
    }
  }

  if(mode == 1){
    for (byte i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 255, 255, 255);    //turn every third pixel on
    }
    strip.setBrightness(brightness);
    strip.show();
  } else if(mode == 2){
    for (byte i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(wheelVal));
    }
    strip.show();
  } else if(mode == 3){
    if(currentMillis - lastMillis > tempo){
      lastMillis = currentMillis;
      for(byte i=0; i< strip.numPixels(); i++) {
        cycleWheelVal = cycleWheelVal + 1 > 255 ? 0 : cycleWheelVal + 1;
        strip.setPixelColor(i, Wheel(cycleWheelVal));
      }
      strip.show();
    }
  }
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
