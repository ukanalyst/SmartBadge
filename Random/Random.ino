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

#define WAIT 10
#define MAXBRIGHT 40


// This function sets up the ledsand tells the controller about them
void setup()
{
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  allOff();
  pulse(30, MAXBRIGHT, 255,0, 0);
  pulse(30, MAXBRIGHT, 0,255, 0);
  pulse(30, MAXBRIGHT, 0,0, 255);
}

// This function runs over and over, and is where you do the magic to light
// your leds.
void loop()
{
  int idx = int(random(0, NUM_LEDS));
  leds[idx].r = int(random(0,MAXBRIGHT));
  leds[idx].g = int(random(0,MAXBRIGHT));
  leds[idx].b = int(random(0,MAXBRIGHT));
  FastLED.show();
  delay(WAIT);
}

void allOff()
{
  for (int i=0; i<NUM_LEDS; i++)
    leds[i] = 0;
  FastLED.show();
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
