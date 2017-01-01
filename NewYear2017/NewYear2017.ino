///////////////////////////////////////////////////////////////////////////////////////////
//
// SmartBadge New Year 2017 Dsiaplay Program
//
// Author:        Dave Almond
// Created:       2016-12-30
// Last Modified: 2016-12-30
// Version :      1.0
//    
///////////////////////////////////////////////////////////////////////////////////////////

#include "FastLED.h"

// Define number of LEDs on the SmartBadge
#define NUM_LEDS 19

// Data pin that LED data will be written out over
#define DATA_PIN 13

// This is an array of LEDs.  One item for each LED
CRGB leds[NUM_LEDS];
boolean cover[NUM_LEDS];  // array showing which LEDs have been visited

#define WAIT 100
#define MAXBRIGHT 40  // Keep brightness down to save blinding people and extend battery life

// Pin numbers for input switches
#define SW1 7
#define SW2 8
#define SW3 9
#define SW4 10

byte chargen[][19] = {
{1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,0,1,1,0}, // 0
{1,1,0,0,0,0,0,1,0,0,0,0,1,0,0,1,0,0,1}, // 1
{1,1,1,0,0,1,1,1,0,0,0,0,0,1,1,0,0,0,0}, // 2
{1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,0,0,0,1}, // 3
{1,0,0,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,0}, // 4
{1,1,0,0,1,1,1,1,0,0,1,1,0,1,0,0,1,0,1}, // 5
{1,1,0,0,1,1,1,1,1,0,1,1,0,1,0,0,1,0,1}, // 6
{1,1,0,0,0,0,0,1,0,0,0,1,1,0,0,1,0,0,1}, // 7
{1,1,1,0,1,1,1,1,1,0,1,1,0,1,0,0,1,0,1}, // 8
{1,1,1,0,1,1,1,1,0,0,1,1,0,1,0,0,1,0,1}, // 9
{1,1,1,0,0,0,1,0,1,0,0,0,0,1,1,0,1,1,1}, // A
{1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,0,1,1,1}, // B
{1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,1,1,0}, // C
{1,1,1,1,1,1,1,1,0,0,0,1,0,0,0,0,1,0,0}, // D
{1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,1}, // E
{1,1,1,0,0,0,0,0,1,0,0,0,0,0,1,0,1,1,1}, // F
{1,1,1,0,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1}, // G
{1,0,1,0,0,0,1,0,1,0,0,0,0,1,1,0,1,1,1}, // H
{1,1,1,0,0,0,1,1,1,0,0,0,1,0,0,1,0,0,1}, // I
{0,1,0,0,0,0,0,1,1,1,1,0,1,0,0,1,0,0,1}, // J
{1,1,0,0,0,0,0,0,1,0,0,0,1,0,1,0,1,1,1}, // K
{1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,1,0}, // L
{1,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,1}, // M
{1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,0,1}, // N
{1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,0,1,1,0}, // O
{1,1,1,0,0,0,0,0,1,0,0,0,0,1,0,0,1,1,1}, // P
{1,1,1,0,0,1,1,1,1,0,0,0,0,1,1,0,1,1,0}, // Q
{1,1,1,0,0,0,1,0,1,0,0,0,0,1,0,1,1,1,1}, // R
{1,1,1,1,0,0,1,1,1,1,0,0,0,0,1,0,0,1,1}, // S
{1,0,0,0,0,0,0,1,0,0,0,0,1,1,0,1,0,0,1}, // T
{1,0,1,0,0,0,1,1,1,0,0,0,0,1,1,0,1,1,0}, // U
{1,0,1,0,0,0,1,1,1,0,0,0,0,1,1,0,1,1,0}, // V
{0,1,0,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,1}, // W
{0,1,0,0,0,1,0,1,0,0,0,1,1,0,1,1,0,1,1}, // X
{1,0,1,0,0,0,0,1,1,0,0,0,0,1,0,1,0,1,1}, // Y
{1,1,1,0,0,1,1,1,1,0,0,1,0,1,0,0,1,0,1}, // Z
};

// Initial state of buttons. HIGH is OFF, LOW is ON
int last1 = HIGH;
int last2 = HIGH;
int last3 = HIGH;
int last4 = HIGH;

// This function sets up the leds and tells the controller about them
void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);

  pinMode(SW1, INPUT);
  digitalWrite(SW1, HIGH);    // pullup resistors required for switches
  pinMode(SW2, INPUT);
  digitalWrite(SW2, HIGH);    
  pinMode(SW3, INPUT);
  digitalWrite(SW3, HIGH);    
  pinMode(SW4, INPUT);
  digitalWrite(SW4, HIGH);    

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(MAXBRIGHT);     //
  // Test each LED with each colour
  for (int i=0; i<NUM_LEDS; i++)
  {
    leds[i] = CRGB(255,0,0);
    FastLED.show();
    delay(50);
    leds[i] = CRGB(0,255,0);
    FastLED.show();
    delay(50);
    leds[i] = CRGB(0,0,255);
    FastLED.show();
    delay(50);
    allOff();
  }
  for (int i=0; i<NUM_LEDS; i++)
  {
    leds[i] = CRGB(255,0,0);
    FastLED.show();
    delay(20);
  }
    delay(1000);
    allOff();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (testButton1()) {
    displayDigit(2,CRGB::Blue);
    delay(1000);
    allOff();
    displayDigit(0,CRGB::BlueViolet);
    delay(1000);
    allOff();
    displayDigit(1,CRGB::Chartreuse);
    delay(1000);
    allOff();
    displayDigit(7,CRGB::DarkOrange);
    delay(1000);
    allOff();
  } else if (testButton2()) {
    for (int i=0; i<NUM_LEDS; i++)
    {
      leds[i] = CRGB(0,255,0);
      FastLED.show();  
      delay(1000);
      leds[i] = 0;
    }
     FastLED.show();       
  } else if (testButton3()) {
    for (int i=0; i<10; i++)
    {
      displayDigit(i,CRGB(0,255,0));
      delay(1000);
    }
    allOff();      
  } 
}

void displayDigit(int digit, CRGB col)
{
  for (int i=0; i<NUM_LEDS; i++)
  {
    if (chargen[digit][i] > 0)
      leds[i] = col;
    else
      leds[i] = 0;
  }
  FastLED.show();
}

boolean testButton1()
{
    int newB = digitalRead(SW1);
    if (newB==LOW and last1==HIGH)
    {
      last1 = LOW;
      return true;
    }
    last1 = newB;
    return false;
}

boolean testButton2()
{
    int newB = digitalRead(SW2);
    if (newB==LOW and last2==HIGH)
    {
      last2 = LOW;
      return true;
    }
    last2 = newB;
    return false;
}

boolean testButton3()
{
    int newB = digitalRead(SW3);
    if (newB==LOW and last3==HIGH)
    {
      last3 = LOW;
      return true;
    }
    last3 = newB;
    return false;
}

boolean testButton4()
{
    int newB = digitalRead(SW4);
    if (newB==LOW and last4==HIGH)
    {
      last4 = LOW;
      return true;
    }
    last4 = newB;
    return false;
}

void allOff()
{
  for (int i=0; i<NUM_LEDS; i++)
    leds[i] = 0;
  FastLED.show();
}

