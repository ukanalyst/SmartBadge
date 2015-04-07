///////////////////////////////////////////////////////////////////////////////////////////
//
// SmartBadge Demo Program 
//
// Button 1 selects the demo to run
// Number of pixels lit shows Demo selected. Starts running after 5? seconds
// Demo 1:  Flash LEDs in different patters. Use Buttons 2..4 to select pattern
// Demo 2:  Play "Fill My Badge" Game. Use Button 4 to capture a pixel
// Demo 3:  DeathRay
// Demo 4:  BugChase
//    
///////////////////////////////////////////////////////////////////////////////////////////

#include "FastSPI_LED2.h"

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
  {1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,0,1,1,0},   // 0
  {1,1,0,0,0,0,0,1,0,0,0,0,1,0,0,1,0,0,1},   // 1
  {1,1,1,0,0,1,1,1,0,0,0,0,0,1,1,0,0,0,0},   // 2
  {1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,0,0,0,1},   // 3
  {1,0,0,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,0},   // 4
  {1,1,0,0,1,1,1,1,0,0,1,1,0,1,0,0,1,0,1},   // 5
  {1,1,0,0,1,1,1,1,0,0,1,1,0,1,0,0,1,0,1},   // 6
  {1,1,0,0,1,1,1,1,0,0,1,1,0,1,0,0,1,0,1},   // 7
  {1,1,0,0,1,1,1,1,0,0,1,1,0,1,0,0,1,0,1},   // 8
  {1,1,0,0,1,1,1,1,0,0,1,1,0,1,0,0,1,0,1}};  // 9

int ring = 0;  // initial ring
int mode = 0;  // Mode represents the demo program running

// Initial state of buttons. HIGH is OFF, LOW is ON
int last1 = HIGH;
int last2 = HIGH;
int last3 = HIGH;
int last4 = HIGH;

// This function sets up the leds and tells the controller about them
void setup()
{
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
  for (int i=5; i>=0; i--)
  {
    displayDigit(i, CRGB(0,255,0));
    delay(1000);
  }
}

// Main loop. Check buttons and move LEDs if needed
void loop()
{
  switch (mode)
  {
    case 0:  // menu select
      mode = 1;
      selectMode();
      break;
    case 1:  // LED effects demo
      demo_mode();
      selectMode();
      break;
    case 2:  // Capture Pixels Game
      FillBadgeGame();
      selectMode();
      break;
    case 3:  // Death Ray
      deathRay();
      selectMode();
      break;
    case 4:  // BugChase - maybe
      bugChase();
      selectMode();
      break;
    default:
      mode = 0;
      break;
  }
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

void selectMode()
{
  int newB;
  unsigned long timeNow = millis();
  while (millis() < timeNow + 5000)  // wait 5 seconds then start running new selection
  {
    dispMode(255);
    if (testButton1())
    {
      if (++mode > 6)
        mode = 1;
      timeNow = millis();
    }
    if (testButton4())  // quick exit from menu
      break; 
  }
  for (int i=240; i>=0; i-=10)  // Fade out the mode counter
  {
    dispMode(i);
    delay(20);
  }
}

void dispMode(int brVal)
{
  // display mode count in middle ring
  CRGB col = CRGB(0,brVal,brVal);
  for (int i=0; i< NUM_LEDS; i++)
    leds[i] = 0;
  for (int i=0; i<6; i++)
    if (mode>i)
      leds[12+i] = col;
  FastLED.show();
}

void FillBadgeGame()
{  // add LED for a short time, Press button 4 while it is still lit to capture it
  // pressing button when LED is NOT lit will reset the count
  // levels get quicker as time goes on
  unsigned long timeNow;
  int level = 0;
  int pos = 0;
  int blinkStart = random(1000, 3000);
  boolean success = true;
  allOff();
  while (success)
  {
    if (testButton1())
      return;
    displayFill(level, pos);
    timeNow = millis();
    while (millis() < timeNow + blinkStart)
    {
      if (testButton1())
        return;
      if (testButton4())
      {
        success = false;
        break;
      }
    }
    if (! success)
    {
      showFail(level);
      return;
    }
    leds[pos] = CRGB(255,255,255);
    FastLED.show();
    timeNow = millis();
    success = false;
    while (millis() < timeNow + (5-level)*200)  // wait a short while then fail
    {
      if (testButton1())
        return;
      if (testButton4())
      {
        success = true;
        break;
      }
    }
    pos++;
    if (pos >= NUM_LEDS)
    {
      pos = 0;
      levelSuccess(level);  // flash some LEDs
      level++;
    }
  }
  // only get here if fail
  showFail(level);
}

void deathRay()
{
  ray(100, CRGB(0,255,0), 3);
      if (testButton1())
        return;
  ray(100, CRGB(255,0,0), 3);
      if (testButton1())
        return;
  ray(100, CRGB(0,0,255), 3);
      if (testButton1())
        return;
  ray(100, CRGB(0,255,255), 3);
      if (testButton1())
        return;
  ray(100, CRGB(255,255,0), 3);
      if (testButton1())
        return;
  ray(100, CRGB(255,255,255), 3);
      if (testButton1())
        return;
  FastLED.setBrightness(255);  // move to single blinder!
  ray(100, CRGB(255,255,255), 5);
/*  for (int i = 0; i<NUM_LEDS; i++)  // only include if your battery is good condition!
    leds[i]=CRGB(255,255,255);
  FastLED.show();
  delay(600);
  allOff();*/
  FastLED.setBrightness(MAXBRIGHT);
}

void ray (int rate, CRGB col, int count)
{
  for (int i = 0; i < count; i++)
  {
    setAllOff();
    setRing3(col);
    FastLED.show();
    delay(rate);
    setRing3(0);
    setRing2(col);
    FastLED.show();
    delay(rate);
    setRing2(0);
    setRing1(col);
    FastLED.show();
    delay(rate);
  }
}

void levelSuccess(int level)
{
  CRGB col = colLevel(level);
  ray (100, col, 5);
}

void setAllOff()
{
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = 0;
}

void displayFill(int level, int pos)
{
  CRGB col = colLevel(level);
  for (int i=0; i<NUM_LEDS; i++)
    if (i<pos)
      leds[i] = col;
    else
      leds[i] = 0;
  FastLED.show();
}

CRGB colLevel(int level)
{
  CRGB col;
  switch(level)
  {
    case 0:
      col = CRGB(0,255,0);
      break;
    case 1:
      col = CRGB(255,0,0);
      break;
    case 2:
      col = CRGB(0,0,255);
      break;
    case 3:
      col = CRGB(255,255,0);
      break;
    case 4:
      col = CRGB(0,255,255);
      break;
    case 5:
      col = CRGB(255,255,255);
      break;
  }
  return col;
}

void showFail(int level)
{
  CRGB col = colLevel(level);
  for (int i=NUM_LEDS-1; i>=0; i--)
    leds[i] = col;
  for (int i=0; i<NUM_LEDS; i++)
  {
    FastLED.show();
    delay(100);
    leds[i] = 0;
  }
}

void bugChase()
{
  // Bug is Red and starts in centre
  // You are Blue and start in led[0] and need cover all the pixels in green
  // SW4 moves anticlockwise
  // SW3 moves clockwise
  // SW2 moves inwards and wraps to outside
  // SW1 exits
  setAllOff();
  for (int i=0; i<NUM_LEDS; i++)
    cover[i] = false;
  int bug = 18, player = 1;  // Starting positions
  int level = 0;  // speed of bug increased as level changes
  CRGB bugCol = CRGB(0,255,0), playerCol = CRGB(0,0,255), coverCol = CRGB(30,0,0);
  boolean success = false;
  unsigned long timeNow = millis();
  showBugPlayer(bug, player, bugCol, playerCol, coverCol);
  int delays = 800;
  while (true)
  {
    while (true)
    {
      if (testButton1())
        return;
      if (testButton2())
      {
        player = moveIn(player);
        success = showBugPlayer(bug, player, bugCol, playerCol, coverCol);
      }
      if (testButton3())
      {
        player = moveClockwise(player);
        success = showBugPlayer(bug, player, bugCol, playerCol, coverCol);
      }
      if (testButton4())
      {
        player = moveAntiClockwise(player);
        success = showBugPlayer(bug, player, bugCol, playerCol, coverCol);
      }
      if (bug == player)  // collided?
        break;  // fail
      if (millis() - timeNow > delays)
      {
        bug = moveBug(bug, player);
        timeNow = millis();
        showBugPlayer(bug, player, bugCol, playerCol, coverCol);
      }
      if (success)
        break;
    }
    if (success)
    {
      ray(100, playerCol, 5);
      level++;
      delays = (delays * 3) / 4; // 
      switch (level)
      {
        case 1:
          coverCol = CRGB(0,30,0);
          break;
        case 2:
          coverCol = CRGB(0,0,30);
          break;
        case 3:
          coverCol = CRGB(30,30,0);
          break;
        case 4:
          coverCol = CRGB(30,0,30);
          break;
        case 5:
          coverCol = CRGB(0,30,30);
          break;
        case 6:
          coverCol = CRGB(30,30,30);
          break;
        default:
          coverCol = CRGB(15,45,62);
          break;
      }
      FastLED.setBrightness(255);     //
      displayDigit(level+1, coverCol);
      FastLED.setBrightness(MAXBRIGHT);     //
      delay(1000);
      bug = 18;
      player = 1;
      clearCovers();
      success = false;
      showBugPlayer(bug, player, bugCol, playerCol, coverCol);
      delay(1000);
    }
    else
      break;
  }
  showFail(0);
}

boolean showBugPlayer(int bug, int player, CRGB bugCol, CRGB playerCol, CRGB coverCol)
{
  for (int i=0; i<NUM_LEDS; i++)
    if (cover[i])
      leds[i] = coverCol;
    else
      leds[i] = 0;
  leds[bug] = bugCol;
  leds[player] = playerCol;
  cover[player] = true;
  FastLED.show();
  return (countCovers() == 19);
}

int countCovers()
{
  int count=0;
  for (int i=0; i<NUM_LEDS; i++)
    if (cover[i])
      count++;
  return count;
}

int clearCovers()
{
  for (int i=0; i<NUM_LEDS; i++)
    cover[i] = false;
}

int moveBug(int bug, int player)
{
  int rval, rPlayer = whichRing(player), rBug = whichRing(bug);
  int lDif, rDif;
  // first check if on same ring. Bug always moves out. Player always moves in
  if (rBug == rPlayer)
  {
    if (rBug == 3)  // inner ring so don't move
      rval = bug;
    else if (rBug == 2)  // middle ring
    {
      lDif = bug-player;
      rDif = player-bug;
      if (lDif<0)
        lDif += 6;
      if (rDif<0)
        rDif += 6;
      if(lDif > rDif)
      {
        rval = bug+1;
        if (rval > 17)
          rval = 12;
      }
      else
      {
        rval = bug-1;
        if (rval < 12)
          rval = 17;
      }
    }
    else  // outer ring
    {
      lDif = bug-player;
      rDif = player-bug;
      if (lDif<0)
        lDif += 12;
      if (rDif<0)
        rDif += 12;
      if(lDif > rDif)
      {
        rval = bug+1;
        if (rval > 11)
          rval = 0;
      }
      else
      {
        rval = bug-1;
        if (rval < 0)
          rval = 11;
      }
    }
  }
  else  // move bug out one ring
  {
    if (rBug == 3)  // in the cente so move to middle ring
    {
      if (rPlayer == 2)
      {
        rval = player + 3;
        if (rval > 17)
          rval -= 6;
      }
      else
        rval = 12 + player/2;
    }
    else if (rBug == 2)  // middle ring so move to outer
      rval = (bug-12) * 2;  // move out to nearest even number
    else  // on outside ring so move to centre
      rval = 18;
  }
  return rval;
}

int whichRing(int pos)
{
  if (pos > 17)
    return 3;
  else if (pos > 11)
    return 2;
  else
    return 1;
}

int moveIn(int pos)
{
  if (pos < 12)  // outer ring
    return 12 + (pos-1)/2;
  if (pos < 18)  // middle ring
    return 18;
  return 1;
}

int moveClockwise(int pos)
{
  int x;
  if (pos<12)  // outer ring
  {
    x = pos+1;
    if (x > 11)
      x = 0;
  }
  else if (pos<18)
  {
    x = pos+1;
    if (x>17)
      x = 12;
  }
  else
    x = 18;
  return x;
}

int moveAntiClockwise(int pos)
{
  int x;
  if (pos<12)  // outer ring
  {
    x = pos-1;
    if (x < 0)
      x = 11;
  }
  else if (pos<18)
  {
    x = pos-1;
    if (x<12)
      x = 17;
  }
  else
    x = 18;
  return x;
}



void hexRing()
{
  int button = getButton();
  switch (button)
  {
    case 1:
      clockwise();
      FastLED.show();
      break;
    case 2:
      anticlockwise();
      FastLED.show();
      break;
    case 3:
      ring = min (2, ring+1);
      break;
    case 4:
      ring = max (0, ring-1);
      break;
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

void clockwise()
{
  if (ring == 0)
    rotateRing1CW();
  else if (ring == 1)
    rotateRing2CW();
}

void anticlockwise()
{
  if (ring == 0)
    rotateRing1CCW();
  else if (ring == 1)
    rotateRing2CCW();
}

int getButton()
{
  int newB = digitalRead(SW1);
  if (newB==LOW and last1==HIGH)
  {
    last1 = LOW;
    return 1;
  }
  last1 = newB;
  newB = digitalRead(SW2);
  if (newB==LOW and last2==HIGH)
  {
    last2 = LOW;
    return 2;
  }
  last2 = newB;
  newB = digitalRead(SW3);
  if (newB==LOW and last3==HIGH)
  {
    last3 = LOW;
    return 3;
  }
  last3 = newB;
  newB = digitalRead(SW4);
  if (newB==LOW and last4==HIGH)
  {
    last4 = LOW;
    return 4;
  }
  last4 = newB;
  return 0;
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

void setAll(CRGB col)
{
  for (int i=0; i<NUM_LEDS; i++)
    leds[i] = col;
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




/*------------------------------------#
#----- FASTSPI2 EFFECTS EXAMPLES -----#
#------------ teldredge --------------#
#-------- www.funkboxing.com ---------#
#------ teldredge1979@gmail.com ------#
#-------------------------------------#
### FAST_SPI2 LED FX EXAMPLES
### MOSTLY DESIGNED FOR A LOOP/RING OF LEDS (ORIGINALLY FOR A SIGN)
### BUT PLENTY USEFUL FOR OTHER CONFIGURATIONS
### RECENT CHANGES:
    -NO LONGER USES SERIALCOMMAND LIBRARY
    -USES THE FAST_SPI2 FOR HSV TO RGB CONVERSION
    -ADDED 'SET MAX BRIGHTNESS' COMMAND [b(0-255)] (NO COMMA)  
### NOTES  
    -MAKE SURE YOU ARE USING FAST_SPI RC3 OR LATER
    -THIS IS AN EXAMPLE LIBRARY SO YOU'LL HAVE TO EDIT TO DO ANYTHING USEFUL WITH IT
    -GOTO FUNKBOXING FAST_SPI2 COMMENTS PAGE FOR HELP
    -DEMO MODE BLOCKS SERIAL COMMANDS
### LICENSE:::USE FOR WHATEVER YOU WANT, WHENEVER YOU WANT, HOWEVER YOU WANT, WHYEVER YOU WANT
### BUT YOU MUST YODEL ONCE FOR FREEDOM AND MAYBE DONATE TO SOMETHING WORTHWHILE
*/

/*----------------------------------------------------|
|                                                     |
|           FROM THE FAST_SPI2 EXAMPLE FILE           |
|                                                     |
|----------------------------------------------------*/
// Uncomment this line if you have any interrupts that are changing pins - this causes the library to be a little bit more cautious
// #define FAST_SPI_INTERRUPTS_WRITE_PINS 1

// Uncomment this line to force always using software, instead of hardware, SPI (why?)
// #define FORCE_SOFTWARE_SPI 1

// Uncomment this line if you want to talk to DMX controllers
// #define FASTSPI_USE_DMX_SIMPLE 1
//-----------------------------------------------------------------------------------------



//#include <FastSPI_LED2.h>

//---LED SETUP STUFF
#define LED_COUNT 19
//#define CLOCK_PIN 13
//#define DATA_PIN 13
//#define DATA_RATE 3

int BOTTOM_INDEX = 0;
int TOP_INDEX = int(LED_COUNT/2);
int EVENODD = LED_COUNT%2;
//struct CRGB leds[LED_COUNT];
int ledsX[LED_COUNT][3];     //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, MARCH, ETC)
int ledMode = 888;             //-START IN RAINBOW LOOP

//---SERIAL/BLUETOOTH SETUP STUFF
#define SERIAL_BAUDRATE 9600
byte inbyte;                 //-SERIAL INPUT BYTE
int thisarg;                 //-SERIAL INPUT ARG

//---LED FX VARS
int max_bright = 20;         //-SET MAX BRIGHTNESS TO 1/4
int idex = 0;                //-LED INDEX (0 to LED_COUNT-1
int idx_offset = 0;          //-OFFSET INDEX (BOTTOM LED TO ZERO WHEN LOOP IS TURNED/DOESN'T REALLY WORK)
int ihue = 0;                //-HUE (0-360)
static uint8_t startinghue = 0;
int ibright = 0;             //-BRIGHTNESS (0-255)
int isat = 0;                //-SATURATION (0-255)
int bouncedirection = 0;     //-SWITCH FOR COLOR BOUNCE (0-1)
float tcount = 0.0;          //-INC VAR FOR SIN LOOPS
int lcount = 0;              //-ANOTHER COUNTING VAR



//------------------------------------- UTILITY FXNS --------------------------------------
//---SET THE COLOR OF A SINGLE RGB LED
void set_color_led(int adex, int cred, int cgrn, int cblu) {  
  int bdex;  
  if (idx_offset > 0) {  //-APPLY INDEX OFFSET 
    bdex = (adex + idx_offset) % LED_COUNT;
  }
  else {bdex = adex;}
  leds[bdex].r = cred;
  leds[bdex].g = cgrn;
  leds[bdex].b = cblu;  
  }

//---FIND INDEX OF HORIZONAL OPPOSITE LED
int horizontal_index(int i) {
  //-ONLY WORKS WITH INDEX < TOPINDEX
  if (i == BOTTOM_INDEX) {return BOTTOM_INDEX;}
  if (i == TOP_INDEX && EVENODD == 1) {return TOP_INDEX + 1;}
  if (i == TOP_INDEX && EVENODD == 0) {return TOP_INDEX;}
  return LED_COUNT - i;  
}

//---FIND INDEX OF ANTIPODAL OPPOSITE LED
int antipodal_index(int i) {
  //int N2 = int(LED_COUNT/2);
  int iN = i + TOP_INDEX;
  if (i >= TOP_INDEX) {iN = ( i + TOP_INDEX ) % LED_COUNT; }
  return iN;
}

//---FIND ADJACENT INDEX CLOCKWISE
int adjacent_cw(int i) {
  int r;
  if (i < LED_COUNT - 1) {r = i + 1;}
  else {r = 0;}
  return r;
}

//---FIND ADJACENT INDEX COUNTER-CLOCKWISE
int adjacent_ccw(int i) {
  int r;
  if (i > 0) {r = i - 1;}
  else {r = LED_COUNT - 1;}
  return r;
}

void copy_led_array(){
  for(int i = 0; i < LED_COUNT; i++ ) {
    ledsX[i][0] = leds[i].r;
    ledsX[i][1] = leds[i].g;
    ledsX[i][2] = leds[i].b;
  }  
}



//------------------------LED EFFECT FUNCTIONS------------------------
void one_color_all(int cred, int cgrn, int cblu) {       //-SET ALL LEDS TO ONE COLOR
    for(int i = 0 ; i < LED_COUNT; i++ ) {
      set_color_led(i, cred, cgrn, cblu);
      LEDS.show();       
      delay(1);
    }  
}

void rainbow_fade(int idelay) {                         //-m2-FADE ALL LEDS THROUGH HSV RAINBOW
    ihue++;
    if (ihue >= 359) {ihue = 0;}
    for(int idex = 0 ; idex < LED_COUNT; idex++ ) {
      leds[idex] = CHSV(ihue, 255, 255);
    }
    LEDS.show();    
    delay(idelay);
}

void rainbow_loop(int istep, int idelay) {              //-m3-LOOP HSV RAINBOW
  idex++;
  ihue = ihue + istep;
  if (idex >= LED_COUNT) {idex = 0;}
  if (ihue >= 359) {ihue = 0;}
  leds[idex] = CHSV(ihue, 255, 255);
  LEDS.show();
  delay(idelay);
}

void random_burst(int idelay) {                         //-m4-RANDOM INDEX/COLOR
  idex = random(0,LED_COUNT);
  ihue = random(0,359);  
  leds[idex] = CHSV(ihue, 255, 255);
  LEDS.show();
  delay(idelay);
}

void color_bounce(int idelay) {                        //-m5-BOUNCE COLOR (SINGLE LED)
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == LED_COUNT) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }  
  for(int i = 0; i < LED_COUNT; i++ ) {
    if (i == idex) {set_color_led(i, 255, 0, 0);}
    else {set_color_led(i, 0, 0, 0);}
  }
  LEDS.show();
  delay(idelay);
}

void color_bounceFADE(int idelay) {                    //-m6-BOUNCE COLOR (SIMPLE MULTI-LED FADE)
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == LED_COUNT) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }
  int iL1 = adjacent_cw(idex);
  int iL2 = adjacent_cw(iL1);
  int iL3 = adjacent_cw(iL2);
  int iR1 = adjacent_ccw(idex);
  int iR2 = adjacent_ccw(iR1);
  int iR3 = adjacent_ccw(iR2);
  for(int i = 0; i < LED_COUNT; i++ ) {
    if (i == idex) {set_color_led(i, 255, 0, 0);}
    else if (i == iL1) {set_color_led(i, 100, 0, 0);}
    else if (i == iL2) {set_color_led(i, 50, 0, 0);}
    else if (i == iL3) {set_color_led(i, 10, 0, 0);}        
    else if (i == iR1) {set_color_led(i, 100, 0, 0);}
    else if (i == iR2) {set_color_led(i, 50, 0, 0);}
    else if (i == iR3) {set_color_led(i, 10, 0, 0);}    
    else {set_color_led(i, 0, 0, 0);}
  }
  LEDS.show();
  delay(idelay);
}

void police_lightsONE(int idelay) {                    //-m7-POLICE LIGHTS (TWO COLOR SINGLE LED)
  idex++;
  if (idex >= LED_COUNT) {idex = 0;}
  int idexR = idex;
  int idexB = antipodal_index(idexR);  
  for(int i = 0; i < LED_COUNT; i++ ) {
    if (i == idexR) {set_color_led(i, 255, 0, 0);}
    else if (i == idexB) {set_color_led(i, 0, 0, 255);}    
    else {set_color_led(i, 0, 0, 0);}
  }
  LEDS.show();  
  delay(idelay);
}

void police_lightsALL(int idelay) {                  //-m8-POLICE LIGHTS (TWO COLOR SOLID)
  idex++;
  if (idex >= LED_COUNT) {idex = 0;}
  int idexR = idex;
  int idexB = antipodal_index(idexR);
  set_color_led(idexR, 255, 0, 0);
  set_color_led(idexB, 0, 0, 255);
  LEDS.show();  
  delay(idelay);
}

void flicker(int thishue, int thissat) {            //-m9-FLICKER EFFECT
  int random_bright = random(0,255);
  int random_delay = random(10,100);
  int random_bool = random(0,random_bright);
  if (random_bool < 10) {
    for(int i = 0 ; i < LED_COUNT; i++ ) {
      leds[i] = CHSV(thishue, thissat, random_bright);
    }
    LEDS.show();    
    delay(random_delay);
  }
}

void pulse_one_color_all(int ihue, int idelay) {    //-m10-PULSE BRIGHTNESS ON ALL LEDS TO ONE COLOR 
  if (bouncedirection == 0) {
    ibright++;
    if (ibright >= 255) {bouncedirection = 1;}
  }
  if (bouncedirection == 1) {
    ibright = ibright - 1;
    if (ibright <= 1) {bouncedirection = 0;}         
  }  
    for(int idex = 0 ; idex < LED_COUNT; idex++ ) {
      leds[idex] = CHSV(ihue, 255, ibright);
    }
    LEDS.show();    
    delay(idelay);
}

void pulse_one_color_all_rev(int ihue, int idelay) { //-m11-PULSE SATURATION ON ALL LEDS TO ONE COLOR 
  if (bouncedirection == 0) {
    isat++;
    if (isat >= 255) {bouncedirection = 1;}
  }
  if (bouncedirection == 1) {
    isat = isat - 1;
    if (isat <= 1) {bouncedirection = 0;}         
  }  
    for(int idex = 0 ; idex < LED_COUNT; idex++ ) {
      leds[idex] = CHSV(ihue, isat, 255);
    }
    LEDS.show();
    delay(idelay);
}

void fade_vertical(int ihue, int idelay) {        //-m12-FADE 'UP' THE LOOP
  idex++;
  if (idex > TOP_INDEX) {idex = 0;}  
  int idexA = idex;
  int idexB = horizontal_index(idexA);
  ibright = ibright + 10;
  if (ibright > 255) {ibright = 0;}
  leds[idexA] = CHSV(ihue, 255, ibright);
  leds[idexB] = CHSV(ihue, 255, ibright);
  LEDS.show();
  delay(idelay);
}

void random_red() {                            //QUICK 'N DIRTY RANDOMIZE TO GET CELL AUTOMATA STARTED  
  int temprand;
  for(int i = 0; i < LED_COUNT; i++ ) {
    temprand = random(0,100);
    if (temprand > 50) {leds[i].r = 255;}
    if (temprand <= 50) {leds[i].r = 0;}
    leds[i].b = 0; leds[i].g = 0;
  }
  LEDS.show();  
}

void rule30(int idelay) {                      //-m13-1D CELLULAR AUTOMATA - RULE 30 (RED FOR NOW)
  if (bouncedirection == 0) {
    random_red();
    bouncedirection = 1;
  }
  copy_led_array();
  int iCW;
  int iCCW;
  int y = 100;  
  for(int i = 0; i < LED_COUNT; i++ ) {
    iCW = adjacent_cw(i);
    iCCW = adjacent_ccw(i);
    if (ledsX[iCCW][0] > y && ledsX[i][0] > y && ledsX[iCW][0] > y) {leds[i].r = 0;}
    if (ledsX[iCCW][0] > y && ledsX[i][0] > y && ledsX[iCW][0] <= y) {leds[i].r = 0;}
    if (ledsX[iCCW][0] > y && ledsX[i][0] <= y && ledsX[iCW][0] > y) {leds[i].r = 0;}
    if (ledsX[iCCW][0] > y && ledsX[i][0] <= y && ledsX[iCW][0] <= y) {leds[i].r = 255;}
    if (ledsX[iCCW][0] <= y && ledsX[i][0] > y && ledsX[iCW][0] > y) {leds[i].r = 255;}
    if (ledsX[iCCW][0] <= y && ledsX[i][0] > y && ledsX[iCW][0] <= y) {leds[i].r = 255;}
    if (ledsX[iCCW][0] <= y && ledsX[i][0] <= y && ledsX[iCW][0] > y) {leds[i].r = 255;}
    if (ledsX[iCCW][0] <= y && ledsX[i][0] <= y && ledsX[iCW][0] <= y) {leds[i].r = 0;}
  }
  LEDS.show();  
  delay(idelay);
}

void random_march(int idelay) {                 //-m14-RANDOM MARCH CCW
  copy_led_array();
  int iCCW;
  leds[0] = CHSV(random(0,360), 255, 255);
  for(int idex = 1; idex < LED_COUNT ; idex++ ) {
    iCCW = adjacent_ccw(idex);
    leds[idex].r = ledsX[iCCW][0];
    leds[idex].g = ledsX[iCCW][1];
    leds[idex].b = ledsX[iCCW][2];
  }
  LEDS.show();  
  delay(idelay);
}

void rwb_march(int idelay) {                    //-m15-R,W,B MARCH CCW
  copy_led_array();
  int iCCW;
  idex++;
  if (idex > 2) {idex = 0;}
  switch (idex) {
    case 0:
      leds[0].r = 255;
      leds[0].g = 0;
      leds[0].b = 0;
    break;
    case 1:
      leds[0].r = 255;
      leds[0].g = 255;
      leds[0].b = 255;
    break;
    case 2:
      leds[0].r = 0;
      leds[0].g = 0;
      leds[0].b = 255;
    break;
  }
    for(int i = 1; i < LED_COUNT; i++ ) {
      iCCW = adjacent_ccw(i);
      leds[i].r = ledsX[iCCW][0];
      leds[i].g = ledsX[iCCW][1];
      leds[i].b = ledsX[iCCW][2];    
    }
  LEDS.show();  
  delay(idelay);
}

void radiation(int ihue, int idelay) {          //-m16-SORT OF RADIATION SYMBOLISH- 
  int N3  = int(LED_COUNT/3);
  int N6  = int(LED_COUNT/6);  
  int N12 = int(LED_COUNT/12);  
  for(int i = 0; i < N6; i++ ) { //-HACKY, I KNOW...
    tcount = tcount + .02;
    if (tcount > 3.14) {tcount = 0.0;}
    ibright = int(sin(tcount)*255);    
    int j0 = (i + LED_COUNT - N12) % LED_COUNT;
    int j1 = (j0+N3) % LED_COUNT;
    int j2 = (j1+N3) % LED_COUNT;    
    leds[j0] = CHSV(ihue, 255, ibright);
    leds[j1] = CHSV(ihue, 255, ibright);
    leds[j2] = CHSV(ihue, 255, ibright);    
  }    
    LEDS.show();    
    delay(idelay);    
}

void color_loop_vardelay() {                    //-m17-COLOR LOOP (SINGLE LED) w/ VARIABLE DELAY
  idex++;
  if (idex > LED_COUNT) {idex = 0;}
  int di = abs(TOP_INDEX - idex);
  int t = constrain((10/di)*10, 10, 500);
  for(int i = 0; i < LED_COUNT; i++ ) {
    if (i == idex) {
      leds[i] = CHSV(0, 255, 255);
    }
    else {
      leds[i].r = 0; leds[i].g = 0; leds[i].b = 0;
    }
  }
  LEDS.show();  
  delay(t);
}

void white_temps() {                            //-m18-SHOW A SAMPLE OF BLACK BODY RADIATION COLOR TEMPERATURES
  int N9 = int(LED_COUNT/9);
  for (int i = 0; i < LED_COUNT; i++ ) {
    if (i >= 0 && i < N9) {leds[i].r = 255; leds[i].g = 147; leds[i].b = 41;}         //-CANDLE - 1900
    if (i >= N9 && i < N9*2) {leds[i].r = 255; leds[i].g = 197; leds[i].b = 143;}     //-40W TUNG - 2600
    if (i >= N9*2 && i < N9*3) {leds[i].r = 255; leds[i].g = 214; leds[i].b = 170;}   //-100W TUNG - 2850
    if (i >= N9*3 && i < N9*4) {leds[i].r = 255; leds[i].g = 241; leds[i].b = 224;}   //-HALOGEN - 3200
    if (i >= N9*4 && i < N9*5) {leds[i].r = 255; leds[i].g = 250; leds[i].b = 244;}   //-CARBON ARC - 5200
    if (i >= N9*5 && i < N9*6) {leds[i].r = 255; leds[i].g = 255; leds[i].b = 251;}   //-HIGH NOON SUN - 5400
    if (i >= N9*6 && i < N9*7) {leds[i].r = 255; leds[i].g = 255; leds[i].b = 255;}   //-DIRECT SUN - 6000
    if (i >= N9*7 && i < N9*8) {leds[i].r = 201; leds[i].g = 226; leds[i].b = 255;}   //-OVERCAST SKY - 7000
    if (i >= N9*8 && i < LED_COUNT) {leds[i].r = 64; leds[i].g = 156; leds[i].b = 255;}//-CLEAR BLUE SKY - 20000  
  }
  LEDS.show();  
  delay(100);
}

void sin_bright_wave(int ihue, int idelay) {        //-m19-BRIGHTNESS SINE WAVE
  for(int i = 0; i < LED_COUNT; i++ ) {
    tcount = tcount + .1;
    if (tcount > 3.14) {tcount = 0.0;}
    ibright = int(sin(tcount)*255);
    leds[i] = CHSV(ihue, 255, ibright);
    LEDS.show();    
    delay(idelay);
  }
}

void pop_horizontal(int ihue, int idelay) {        //-m20-POP FROM LEFT TO RIGHT UP THE RING
  int ix;
  if (bouncedirection == 0) {
    bouncedirection = 1;
    ix = idex;  
  }
  else if (bouncedirection == 1) {
    bouncedirection = 0;
    ix = horizontal_index(idex);
    idex++;
    if (idex > TOP_INDEX) {idex = 0;}      
  }
  for(int i = 0; i < LED_COUNT; i++ ) {
    if (i == ix) {
      leds[i] = CHSV(ihue, 255, 255);
    }
    else {
      leds[i].r = 0; leds[i].g = 0; leds[i].b = 0;
    }
  }
  LEDS.show();  
  delay(idelay);    
}

void quad_bright_curve(int ihue, int idelay) {      //-m21-QUADRATIC BRIGHTNESS CURVER
  int ax;    
  for(int x = 0; x < LED_COUNT; x++ ) {
    if (x <= TOP_INDEX) {ax = x;}
    else if (x > TOP_INDEX) {ax = LED_COUNT-x;}
    int a = 1; int b = 1; int c = 0;
    int iquad = -(ax*ax*a)+(ax*b)+c; //-ax2+bx+c
    int hquad = -(TOP_INDEX*TOP_INDEX*a)+(TOP_INDEX*b)+c;
    ibright = int((float(iquad)/float(hquad))*255);
    leds[x] = CHSV(ihue, 255, ibright);
  }
  LEDS.show();  
  delay(idelay);
}

void flame() {                                    //-m22-FLAMEISH EFFECT
  int idelay = random(0,35);
  float hmin = 0.1; float hmax = 45.0;
  float hdif = hmax-hmin;
  int randtemp = random(0,3);
  float hinc = (hdif/float(TOP_INDEX))+randtemp;
  int ihue = hmin;
  for(int i = 0; i < TOP_INDEX; i++ ) {
    ihue = ihue + hinc;
    leds[i] = CHSV(ihue, 255, 255);
    int ih = horizontal_index(i);    
    leds[ih] = CHSV(ihue, 255, 255);    
    leds[TOP_INDEX].r = 255; leds[TOP_INDEX].g = 255; leds[TOP_INDEX].b = 255;    
    LEDS.show();    
    delay(idelay);
  }
}

void rainbow_vertical(int istep, int idelay) {        //-m23-RAINBOW 'UP' THE LOOP
  idex++;
  if (idex > TOP_INDEX) {idex = 0;}  
  ihue = ihue + istep;
  if (ihue > 359) {ihue = 0;}
  int idexA = idex;
  int idexB = horizontal_index(idexA);
  leds[idexA] = CHSV(ihue, 255, 255);
  leds[idexB] = CHSV(ihue, 255, 255);
  LEDS.show();  
  delay(idelay);
}

void pacman(int idelay) {                              //-m24-REALLY TERRIBLE PACMAN CHOMPING EFFECT
  int s = int(LED_COUNT/4);
  lcount++;
  if (lcount > 5) {lcount = 0;}
  if (lcount == 0) {
    for(int i = 0 ; i < LED_COUNT; i++ ) {set_color_led(i, 255, 255, 0);}
    }
  if (lcount == 1 || lcount == 5) {
    for(int i = 0 ; i < LED_COUNT; i++ ) {set_color_led(i, 255, 255, 0);}
    leds[s].r = 0; leds[s].g = 0; leds[s].b = 0;}
  if (lcount == 2 || lcount == 4) {
    for(int i = 0 ; i < LED_COUNT; i++ ) {set_color_led(i, 255, 255, 0);}
    leds[s-1].r = 0; leds[s-1].g = 0; leds[s-1].b = 0;  
    leds[s].r = 0; leds[s].g = 0; leds[s].b = 0;
    leds[s+1].r = 0; leds[s+1].g = 0; leds[s+1].b = 0;}        
  if (lcount == 3) {
    for(int i = 0 ; i < LED_COUNT; i++ ) {set_color_led(i, 255, 255, 0);}
    leds[s-2].r = 0; leds[s-2].g = 0; leds[s-2].b = 0;  
    leds[s-1].r = 0; leds[s-1].g = 0; leds[s-1].b = 0;  
    leds[s].r = 0; leds[s].g = 0; leds[s].b = 0;
    leds[s+1].r = 0; leds[s+1].g = 0; leds[s+1].b = 0;
    leds[s+2].r = 0; leds[s+2].g = 0; leds[s+2].b = 0;}
  LEDS.show();  
  delay(idelay);
}

void strip_march_cw(int idelay) {                      //-m50-MARCH STRIP CW
  copy_led_array();
  int iCCW;  
  for(int i = 0; i < LED_COUNT; i++ ) {
    iCCW = adjacent_ccw(i);
    leds[i].r = ledsX[iCCW][0];
    leds[i].g = ledsX[iCCW][1];
    leds[i].b = ledsX[iCCW][2];    
  }
  LEDS.show();  
  delay(idelay);
}

void strip_march_ccw(int idelay) {                      //-m51-MARCH STRIP CCW 
  copy_led_array();
  int iCW;  
  for(int i = 0; i < LED_COUNT; i++ ) {
    iCW = adjacent_cw(i);
    leds[i].r = ledsX[iCW][0];
    leds[i].g = ledsX[iCW][1];
    leds[i].b = ledsX[iCW][2];    
  }
  LEDS.show();  
  delay(idelay);
}

void new_rainbow_loop(int idelay){                       //-m88-RAINBOW FADE FROM FAST_SPI2
  ihue -= 1;
  fill_rainbow( leds, LED_COUNT, ihue );  
  LEDS.show();
  delay(idelay);
}

void demo_mode(){
  int r = 10;
  for(int i=0; i<r*3; i++) {one_color_all(255,255,255); if (testButton1()) return;}
  for(int i=0; i<r*25; i++) {rainbow_fade(20); if (testButton1()) return;}
  for(int i=0; i<r*20; i++) {rainbow_loop(10, 20); if (testButton1()) return;}
  for(int i=0; i<r*20; i++) {random_burst(20); if (testButton1()) return;}
  for(int i=0; i<r*12; i++) {color_bounce(20); if (testButton1()) return;}
  for(int i=0; i<r*12; i++) {color_bounceFADE(40); if (testButton1()) return;}
  for(int i=0; i<r*6; i++) {police_lightsONE(40); if (testButton1()) return;}
  for(int i=0; i<r*5; i++) {police_lightsALL(40); if (testButton1()) return;}
  for(int i=0; i<r*40; i++) {flicker(160, 50); if (testButton1()) return;}
  for(int i=0; i<r*50; i++) {pulse_one_color_all(0, 10); if (testButton1()) return;}
  for(int i=0; i<r*35; i++) {pulse_one_color_all_rev(0, 10); if (testButton1()) return;}
  for(int i=0; i<r*5; i++) {fade_vertical(240, 60); if (testButton1()) return;}
  random_red();
  for(int i=0; i<r*5; i++) {rule30(100); if (testButton1()) return;}
  for(int i=0; i<r*8; i++) {random_march(30); if (testButton1()) return;}
  for(int i=0; i<r*5; i++) {rwb_march(80); if (testButton1()) return;}
  one_color_all(0,0,0);
  for(int i=0; i<r*15; i++) {radiation(360, 60); if (testButton1()) return;}
  for(int i=0; i<r*15; i++) {color_loop_vardelay(); if (testButton1()) return;}
  for(int i=0; i<r*5; i++) {white_temps(); if (testButton1()) return;}
  for(int i=0; i<r; i++) {sin_bright_wave(240, 35); if (testButton1()) return;}
  for(int i=0; i<r*5; i++) {pop_horizontal(0, 100); if (testButton1()) return;}
  for(int i=0; i<r*4; i++) {quad_bright_curve(240, 100); if (testButton1()) return;}
  one_color_all(0,0,0);  
  for(int i=0; i<r*3; i++) {flame(); if (testButton1()) return;}
  for(int i=0; i<r*10; i++) {pacman(50); if (testButton1()) return;}  
  for(int i=0; i<r*15; i++) {rainbow_vertical(15, 50); if (testButton1()) return;}
  for(int i=0; i<r*3; i++) {strip_march_ccw(100); if (testButton1()) return;}
  for(int i=0; i<r*3; i++) {strip_march_cw(100); if (testButton1()) return;}  
  for(int i=0; i<r*3; i++) {one_color_all(255,0,0); if (testButton1()) return;}
  for(int i=0; i<r*3; i++) {one_color_all(0,255,0); if (testButton1()) return;}
  for(int i=0; i<r*3; i++) {one_color_all(0,0,255); if (testButton1()) return;}
  for(int i=0; i<r*3; i++) {one_color_all(255,255,0); if (testButton1()) return;}
  for(int i=0; i<r*3; i++) {one_color_all(0,255,255); if (testButton1()) return;}
  for(int i=0; i<r*3; i++) {one_color_all(255,0,255); if (testButton1()) return;}
  for(int i=0; i<r*120; i++) {new_rainbow_loop(2); if (testButton1()) return;}  
}


