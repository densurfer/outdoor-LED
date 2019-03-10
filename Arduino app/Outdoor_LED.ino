/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <colorpalettes.h>

#include <FastLED.h>
#include <ArduinoJson.h>

#define LED_PIN     4
#define NUM_LEDS    60
#define BRIGHTNESS  64
#define LED_TYPE    WS2813
#define COLOR_ORDER RGB
CRGB leds[NUM_LEDS];

//#define UPDATES_PER_SECOND 100

/* Set these to your desired credentials. */
const char* ssid = "Led control";

ESP8266WebServer server(80);

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/

enum Modus {
  ZWART,
  REGENBOOG,
  ALLES,
  CLOUD,
  PARTY,
  PAG,
  RANDOM
};

int int_msg = 0;
int update_per_second = 100;
DynamicJsonBuffer jsonBuffer;
CRGB color;
bool fullColor = false;

void handleRoot() {
  if (server.hasArg("plain")== false){ //Check if body received
    server.send(200, "text/plain", "Body not received");
    return;
  } 

   JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));
 
  String message = "Body received:\n";
         message += server.arg("plain");
         message += "\n";
         
 int_msg = root[String("modus")];
 update_per_second = root[String("speed")];
 color = CRGB(root["data"][0], root["data"][1], root["data"][2]);
 fullColor = root["fullColor"];
 
  server.send(200, "text/plain", (String) color);
  Serial.println(message);
}

void handleBody() { //Handler for the body path
 
      if (server.hasArg("plain")== false){ //Check if body received
 
            server.send(200, "text/plain", "Body not received");
            return;
 
      }
 
      String message = "Body received:\n";
             message += server.arg("plain");
             message += "\n";
 
      server.send(200, "text/plain", message);
      Serial.println(message);
}

void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
}



void loop() {
  server.handleClient();
  fillLeds();
  if(fullColor) {
    setColor(color);
  } else {
      switchLeds(int_msg);
  }
  
  FastLED.show();
  FastLED.delay( 1000 / update_per_second );

}


void setColor(CRGB toSetColor){
    for(int i = 0; i < NUM_LEDS; i++) { 
      leds[i] =  toSetColor;
     }
}

void switchLeds(int int_modus){
   switch(int_modus){
    case 0:
      setBlack();
      break;
    case 12:
      ChangePalettePeriodically();
      break;   
    case 1: 
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
      break;
    case 2:
        currentPalette = RainbowStripeColors_p;
        currentBlending = NOBLEND;
        break;
      case 3:
        currentPalette = RainbowStripeColors_p;
        currentBlending = LINEARBLEND;
        break;
      case 4: 
        SetupPurpleAndGreenPalette();
        currentBlending = LINEARBLEND; 
        break;
      case 5:
        SetupTotallyRandomPalette();
        currentBlending = LINEARBLEND; 
        break;
      case 6:
        SetupBlackAndWhiteStripedPalette();
        currentBlending = NOBLEND; 
        break;
      case 7:
        SetupBlackAndWhiteStripedPalette();
        currentBlending = LINEARBLEND;
        break;
      case 8:
        currentPalette = CloudColors_p;
        currentBlending = LINEARBLEND; 
        break;
      case 9:
         currentPalette = PartyColors_p;
         currentBlending = LINEARBLEND; 
         break;
      case 10:
        currentPalette = myRedWhiteBluePalette_p;
        currentBlending = NOBLEND; 
        break;
      case 11:
         currentPalette = myRedWhiteBluePalette_p;
         currentBlending = LINEARBLEND; 
         break;
      case 13:
        Fire2012();
        break;
      
  }
}

uint8_t fillLeds(){
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */
  FillLEDsFromPaletteColors( startIndex);
}

void setBlack(){
    for(int i = 0; i < NUM_LEDS; i++) { leds[i] = CRGB::Black; }
}


void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};

void Fire2012()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];
  int COOLING = 55;
  int SPARKING = 120;
 bool gReverseDirection = false;

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
      int red = leds[pixelnumber].green;
      int green = leds[pixelnumber].red;
      leds[pixelnumber].green = red;
      leds[pixelnumber].red = green;
    }
}
