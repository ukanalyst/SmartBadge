SmartBadge v0.2
===============

SmartBadge is an Arduino compatible device with 19 neopixels organised concentric hexagons, and 4 buttons

Each pixel is individually addressable and has 8 bits to define the brightnes of each colour (Red, Green and Blue).

Powered by a rechargeable lithium coin cell which lasts 45-55 minutes per charge

program using the standard Arduino IDE and set your board type to "Arduino UNO"

You will need a compatible USB-serial converter for programming the product. This requires the DTR pin on pin 6 which is used to reset the device ready for programming. Currently this is not working when used with a Mac

The included demo uses SW1 as the menu button and has 4 separate flashing demos and games:
SW1 acts as the menu button. After a 5,4,3,2,1,0 countdown it shows a single purple LED. Pressing SW1 cycles through 1,2,3,4,5,6 LEDs lit. It times out after 5 seconds without a button press and runs the selected demo as below. You can press SW4 if you don’t want to wait the 5 seconds.

Press SW1 at any point to return to the menu

Individual Demos

1. This is the default demo and just shows lots of pretty patterns

2. Capture Pixels game. You have to press SW4 when the white LED is on to capture it.
	When you complete the level it starts again, but you get less time to do it

3. Death Ray. Ray gun effect with super bright finale

4. Bug Chase. My brother and I wrote this game at the end of the 70s/early 80s (ancient history). It 	sort of pre-dates pacman. You (Blue) have to visit every pixel to collect them, whilst being chased by a bug (Red). SW4 and SW3 move you around the hexagonal ring. SW2 moves you inwards. Collected pixels show green on the first level and subsequent levels get faster. On our original game we introduced a second bug on level 3, but I haven’t done that. This game is very hard!

