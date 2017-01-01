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

char chargen[][21] = {
{"01110001110000110110"}, // 0
{"11100000100001001001"}, // 1
{"21110011100000110000"}, // 2
{"31110001110000110001"}, // 3
{"41000101010000111110"}, // 4
{"51100111100110100101"}, // 5
{"61100111110110100101"}, // 6
{"71100000100011001001"}, // 7
{"81110111110110100101"}, // 8
{"91110111100110100101"}, // 9
{"A1110001010000110111"}, // A
{"B1110001110000110111"}, // B
{"C1110001110000000110"}, // C
{"D1111111100010000100"}, // D
{"E1100000110000000111"}, // E
{"F1110000010000010111"}, // F
{"G1110101111110010001"}, // G
{"H1010001010000110111"}, // H
{"I1110001110001001001"}, // I
{"J0100000111101001001"}, // J
{"K1100000010001010111"}, // K
{"L1000001110000000110"}, // L
{"M1011110001111000001"}, // M
{"N1000000001111001001"}, // N
{"O1110001110000110110"}, // O
{"P1110000010000100111"}, // P
{"Q1110011110000110110"}, // Q
{"R1110001010000101111"}, // R
{"S1111001111000010011"}, // S
{"T1000000100001101001"}, // T
{"U1010001110000110110"}, // U
{"V1010001110000110110"}, // V
{"W0101111111111001001"}, // W
{"X0100010100011011011"}, // X
{"Y1010000110000101011"}, // Y
{"Z1110011110010100101"} // Z
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
  Serial.begin(9600);
  
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
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(255, 0, 0);
    FastLED.show();
    delay(50);
    leds[i] = CRGB(0, 255, 0);
    FastLED.show();
    delay(50);
    leds[i] = CRGB(0, 0, 255);
    FastLED.show();
    delay(50);
    allOff();
  }
/*  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(255, 0, 0);
    FastLED.show();
    delay(20);
  }
  delay(1000);
  allOff();*/
}

void loop() {
  // put your main code here, to run repeatedly:
  if (testButton1()) {
    Serial.println("Button 1");
    displaySymbol('H', CRGB::Blue);
    delay(1000);
    allOff();
    displaySymbol('A', CRGB::Blue);
    delay(1000);
    allOff();
    displaySymbol('P', CRGB::Blue);
    delay(1000);
    allOff();
    displaySymbol('P', CRGB::Blue);
    delay(1000);
    allOff();
    displaySymbol('Y', CRGB::Blue);
    delay(1000);
    allOff();
    displaySymbol('2', CRGB::Blue);
    delay(1000);
    allOff();
    displaySymbol('0', CRGB::BlueViolet);
    delay(1000);
    allOff();
    displaySymbol('1', CRGB::Chartreuse);
    delay(1000);
    allOff();
    displaySymbol('7', CRGB::DarkOrange);
    delay(1000);
    allOff();
  } else if (testButton2()) {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB(0, 255, 0);
      FastLED.show();
      delay(1000);
      leds[i] = 0;
    }
    FastLED.show();
  } else if (testButton3()) {
  }
}

void displaySymbol(char symbol, CRGB col)
{
  Serial.println("in DS");
  for (int s = 0; s < (sizeof(chargen) / sizeof(chargen[0])); s++) {
    Serial.print("array element: ");
    Serial.println(s);
    Serial.println(chargen[s][0]);
    if (chargen[s][0] == symbol) {
      for (int i = 0; i < NUM_LEDS +1; i++)
      {
        if (chargen[s][i] == '1') {
          Serial.print("Setting LED: ");
          Serial.print(i);
          Serial.println(" ON");
          leds[i-1] = col;
        } else {
          leds[i-1] = 0;
          Serial.print("Setting LED: ");
          Serial.print(i);
          Serial.println(" OFF");
        }
      }
      FastLED.show();
    }
  }
}

boolean testButton1()
{
  int newB = digitalRead(SW1);
  if (newB == LOW and last1 == HIGH)
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
  if (newB == LOW and last2 == HIGH)
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
  if (newB == LOW and last3 == HIGH)
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
  if (newB == LOW and last4 == HIGH)
  {
    last4 = LOW;
    return true;
  }
  last4 = newB;
  return false;
}

void allOff()
{
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = 0;
  FastLED.show();
}

