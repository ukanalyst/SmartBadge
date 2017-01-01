#include "FastSPI_LED2.h"

///////////////////////////////////////////////////////////////////////////////////////////
//
// Testing the LEDs on the Lumino ATMega328 robot,
// Sets Red, Green, Blue, White in turn
// 20 LEDs arranged clockwise around the Lumino as:
//    1 at left rear (0)
//    8 on left side (1 to 8 - rear to front)
//    1 on left front (9)
//    1 on right front (10)
//    8 on right side (11 to 18 - front to rear)
//    1 on right rear (19)
//    
///////////////////////////////////////////////////////////////////////////////////////////
 

// How many leds are in the strip?
#define NUM_LEDS 19

// Data pin that led data will be written out over (11 for rev 0.2, 13 for 0.3 onwards)
#define DATA_PIN 13

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

#define WAIT 100
#define MAXBRIGHT 40


// This function sets up the ledsand tells the controller about them
void setup()
{
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  allOff();
  int colour = int(random(0, MAXBRIGHT));
  setRing1(CRGB(MAXBRIGHT,0,0));
  setRing2(CRGB(0,MAXBRIGHT,0));
  setRing3(CRGB(0,0,MAXBRIGHT));
  FastLED.show();
  delay(WAIT);
  leds[0]=CRGB(MAXBRIGHT,MAXBRIGHT,MAXBRIGHT);
  leds[12]=CRGB(MAXBRIGHT,MAXBRIGHT,MAXBRIGHT);
  FastLED.show();
}

// This function runs over and over, and is where you do the magic to light
// your leds.
void loop()
{
  delay(WAIT);
  rotateRing1CCW();
  rotateRing2CW();
  FastLED.show();
}

void allOff()
{
  for (int i=0; i<NUM_LEDS; i++)
    leds[i] = 0;
  FastLED.show();
}

void rotateRing1CCW()
{
  // Ring 1 is LEDs 0..11
  CRGB led0 = leds[0];
  for (int i=0; i<11; i++)
    leds[i] = leds[i+1];
  leds[11] = led0;
}

void rotateRing1CW()
{
  // Ring 1 is LEDs 0..11
  CRGB led11 = leds[11];
  for (int i=11; i>0; i--)
    leds[i] = leds[i-1];
  leds[0] = led11;
}

void rotateRing2CCW()
{
  // Ring 2 is LEDs 12..17
  CRGB led12 = leds[12];
  for (int i=12; i<17; i++)
    leds[i] = leds[i+1];
  leds[17] = led12;
}

void rotateRing2CW()
{
  // Ring 2 is LEDs 12..17
  CRGB led17 = leds[17];
  for (int i=17; i>12; i--)
    leds[i] = leds[i-1];
  leds[12] = led17;
}

void setRing1(CRGB colour)
{
  // Ring 1 is LEDs 0..11
  for(int i=0; i<12; i++)
    leds[i] = colour;
}

void setRing2(CRGB colour)
{
  // Ring 2 is LEDs 12..17
  for(int i=12; i<18; i++)
    leds[i] = colour;
}

void setRing3(CRGB colour)
{
  // Ring 3 is LED 18
  leds[18] = colour;
}

void setAll(int red, int green, int blue)
{
  for (int i=0; i<NUM_LEDS; i++)
  {
    leds[i].g = red;
    leds[i].r = green;
    leds[i].b = blue;
  }
  FastLED.show();
}


void pulse(int del, int maxBright, int red, int green, int blue)
{
  for (int i = 0; i<maxBright; i++)
  {
    for (int j=0; j<NUM_LEDS; j++)
    {
      leds[j].g = (i*red) >> 8;
      leds[j].r = (i*green) >> 8;
      leds[j].b = (i*blue) >> 8;
    }
    FastLED.show();
    delay(del);
  }
  for (int i = maxBright; i>=0; i--)
  {
    for (int j=0; j<NUM_LEDS; j++)
    {
      leds[j].g = (i*red) >> 8;
      leds[j].r = (i*green) >> 8;
      leds[j].b = (i*blue) >> 8;
    }
    FastLED.show();
    delay(del);
  }
}

void flicker()
{
   int i;
   // Move a single white led 
   for(i = 0; i < NUM_LEDS; i++)
   {
      // Turn our current led on to white, then show the leds
      leds[i] = CRGB::Red;

      // Show the leds (only one of which is set to white, from above)
      FastLED.show();

      // Wait a little bit
      delay(100);

      // Turn our current led back to black for the next loop around
      leds[i] = CRGB::Green;
      if(i>0)
        leds[i-1] = CRGB::Black;
   }
   for(--i; i>=0; i--)
   {
     leds[i] = CRGB::Blue;
     FastLED.show();
     delay(100);
     leds[i] = CRGB::Green;
     if(i<NUM_LEDS-1)
       leds[i+1] = CRGB::Black;
   }

}
