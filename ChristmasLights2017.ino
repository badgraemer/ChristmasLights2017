/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
#include <string.h> //from other sketch
#include <Arduino.h> //from other sketch
#include <Adafruit_NeoPixel.h> //from other sketch

#include <bluefruit.h>

/****************
 * neopattern base code
 */

// Pattern types supported:
enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE, FIRE };
// Patern directions supported:
enum  direction { FORWARD, REVERSE };
 
// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
    public:
 
    // Member Variables:  
    pattern  ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern
    
    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position
    
    uint32_t Color1, Color2;  // What colors are in use
    uint16_t TotalSteps;  // total number of steps in the pattern
    uint16_t Index;  // current step within the pattern
    int Loops;  // current pattern loops
    int numLoops = 3;
    boolean nextPattern;
    boolean LoopMode;
    
    void (*OnComplete)();  // Callback on completion of pattern
    
    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
    :Adafruit_NeoPixel(pixels, pin, type)
    {
        OnComplete = callback;
    }
    
    // Update the pattern
    void Update()
    {
        if((millis() - lastUpdate) > Interval) // time to update
        {
            lastUpdate = millis();
            switch(ActivePattern)
            {
                case RAINBOW_CYCLE:
                    RainbowCycleUpdate();
                    break;
                case THEATER_CHASE:
                    TheaterChaseUpdate();
                    break;
                case COLOR_WIPE:
                    ColorWipeUpdate();
                    break;
                case SCANNER:
                    ScannerUpdate();
                    break;
                case FADE:
                    FadeUpdate();
                    break;
                case FIRE:
                    FadeUpdate();
                    break;
                default:
                    break;
            }
        }
    }
  
    // Increment the Index and reset at the end
    void Increment()
    {
       if (Loops > numLoops) {
        nextPattern = true;
        Loops = 0;
        }
        else { 
        if (Direction == FORWARD)
        {
           Index++;
           if (Index >= TotalSteps)
            {
                Index = 0;
                Loops = Loops + 1;
                Serial.print("Loops ");
                Serial.println(Loops);
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
        else // Direction == REVERSE
        {
            --Index;
            if (Index <= 0)
            {
                Index = TotalSteps-1;
                Loops = Loops + 1;
                Serial.print("Loops ");
                Serial.println(Loops);
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
      }
    }
    
    // Reverse pattern direction
    void Reverse()
    {
        if (Direction == FORWARD)
        {
            Direction = REVERSE;
            Index = TotalSteps-1;
        }
        else
        {
            Direction = FORWARD;
            Index = 0;
        }
    }
    
    // Initialize for a RainbowCycle
    void RainbowCycle(uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = RAINBOW_CYCLE;
        Interval = interval;
        TotalSteps = 255;
        Index = 0;
        Loops = 0;
        Direction = dir;
    }
    
    // Update the Rainbow Cycle Pattern
    boolean RainbowCycleUpdate()
    {
        for(int i=0; i< numPixels(); i++)
        {
            setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
        }
        show();
        Increment();
    }
 
    // Initialize for a Theater Chase
    void TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = THEATER_CHASE;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color1;
        Color2 = color2;
        Loops = 0;
        Index = 0;
        Direction = dir;
   }
    
    // Update the Theater Chase Pattern
    void TheaterChaseUpdate()
    {
        for(int i=0; i< numPixels(); i++)
        {
            if ((i + Index) % 3 == 0)
            {
                setPixelColor(i, Color1);
            }
            else
            {
                setPixelColor(i, Color2);
            }
        }
        show();
        Increment();
    }
 /*
    // Initialize for a ColorWipe
    void ColorWipe(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = COLOR_WIPE;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
        Loops = 0;
    }
    
    // Update the Color Wipe Pattern
    void ColorWipeUpdate()
    {
        setPixelColor(Index, Color1);
        show();
        Increment();
    }

    */
    // Initialize for a COLOR_WIPE
    void ColorWipe(uint32_t color1, uint32_t color2, uint8_t interval)
    {
        ActivePattern = COLOR_WIPE;
        Interval = interval;
        TotalSteps = (numPixels() - 1) * 2;
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Loops = 0;
    }
 
    // Update the Color Wipe Pattern
    void ColorWipeUpdate()
    { 
        for (int i = 0; i < numPixels(); i++)
        {
            if (i == Index)  // Scan Pixel to the right
            {
                 setPixelColor(i, Color1);
                 setPixelColor(i+1, Color1);
            }
            else if (i == TotalSteps - Index) // Scan Pixel to the left
            {
                 setPixelColor(i, Color2);
                 setPixelColor(i-1, Color2);
                 setPixelColor(i+1, Color2);
            }
         /*   else // Fading tail
            {
                 setPixelColor(i, DimColor(getPixelColor(i)));
            } */
        }
        show();
        Increment();
    }

    // Initialize for a SCANNNER
    void Scanner(uint32_t color1, uint32_t color2, uint8_t interval)
    {
        ActivePattern = SCANNER;
        Interval = interval;
        TotalSteps = (numPixels() - 1) * 2;
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Loops = 0;
    }
 
    // Update the Scanner Pattern
    void ScannerUpdate()
    { 
        for (int i = 0; i < numPixels(); i++)
        {
            if (i == Index)  // Scan Pixel to the right
            {
                 setPixelColor(i, Color1);
                 setPixelColor(i+1, Color2);
            }
            else if (i == TotalSteps - Index) // Scan Pixel to the left
            {
                 setPixelColor(i, Color1);
                 setPixelColor(i-1, Color2);
            }
            else // Fading tail
            {
                 setPixelColor(i, DimColor(getPixelColor(i)));
            }
        }
        show();
        Increment();
    }
    
    // Initialize for a Fade
    void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = FADE;
        Interval = interval;
        TotalSteps = steps;
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
        Loops = 0;
    }
    
    // Update the Fade Pattern
    void FadeUpdate()
    {
        // Calculate linear interpolation between Color1 and Color2
        // Optimise order of operations to minimize truncation error
        uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
        uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
        uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;
        
        ColorSet(Color(red, green, blue));
        show();
        Increment();
    }
   
    // Calculate 50% dimmed version of a color (used by ScannerUpdate)
    uint32_t DimColor(uint32_t color)
    {
        // Shift R, G and B components one bit to the right
        uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
        return dimColor;
    }
 
    // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color)
    {
        for (int i = 0; i < numPixels(); i++)
        {
            setPixelColor(i, color);
        }
        show();
    }
 
    // Returns the Red component of a 32-bit color
    uint8_t Red(uint32_t color)
    {
        return (color >> 16) & 0xFF;
    }
 
    // Returns the Green component of a 32-bit color
    uint8_t Green(uint32_t color)
    {
        return (color >> 8) & 0xFF;
    }
 
    // Returns the Blue component of a 32-bit color
    uint8_t Blue(uint32_t color)
    {
        return color & 0xFF;
    }
    
    // Input a value 0 to 255 to get a color value.
    // The colours are a transition r - g - b - back to r.
    uint32_t Wheel(byte WheelPos)
    {
        WheelPos = 255 - WheelPos;
        if(WheelPos < 85)
        {
            return Color(255 - WheelPos * 3, 0, WheelPos * 3);
        }
        else if(WheelPos < 170)
        {
            WheelPos -= 85;
            return Color(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        else
        {
            WheelPos -= 170;
            return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
        }
    }

 // fire effect
/* void loop() {
  Fire(55,120,15);
} */
/*
    // Initialize for a Fire
    void Fire(int Cooling, int Sparking, uint8_t interval)
    {
        ActivePattern = FIRE;
        Interval = interval;
        TotalSteps = 255;
        Index = 0;
        Loops = 0;
       // Direction = dir;
    }
    
    // Update the Fire Pattern
    void FireUpdate()
    {
static byte heat[numPixels()];
  int cooldown;
 
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < numPixels(); i++) {
    cooldown = random(0, ((Cooling * 10) / numPixels()) + 2);
   
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= numPixels() - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
   
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < numPixels(); j++) {
    setPixelHeatColor(j, heat[j] );
  }

  //showStrip();
  //delay(SpeedDelay);

        show();
        Increment();
    }
/*
void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;
 
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
   
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
   
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}
*/
/*
void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

*/
};
 
void StickComplete();
 
// Define some NeoPatterns for the two rings and the stick
//  as well as some completion routines
//NeoPatterns Stick(150, 15, NEO_GRB + NEO_KHZ800, &StickComplete);
NeoPatterns Stick(150, 15, NEO_GRBW + NEO_KHZ800, &StickComplete);
//NeoPatterns Stick(21, 15, NEO_GRB + NEO_KHZ800, &StickComplete);

/***********
 * end of neopattern base code
 */

BLEUart bleuart;

// Function prototypes for packetparser.cpp
uint8_t readPacket (BLEUart *ble_uart, uint16_t timeout);
float   parsefloat (uint8_t *buffer);
void    printHex   (const uint8_t * data, const uint32_t numBytes);

// Packet buffer
extern uint8_t packetbuffer[];

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit52 Controller App Example"));
  Serial.println(F("-------------------------------------------"));

  Bluefruit.begin();
  // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(4);
  Bluefruit.setName("Bluefruit52");

  // Configure and start the BLE Uart service
  bleuart.begin();

  // Set up and start advertising
  startAdv();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println(F("Then activate/use the sensors, color picker, game controller, etc!"));
  Serial.println();  

  //from neopattern example
  // Initialize all the pixelStrips

    Stick.begin();

    Stick.nextPattern = false;
    Stick.LoopMode = true;
    
    // Kick off a pattern
  //  Ring1.TheaterChase(Ring1.Color(255,255,0), Ring1.Color(0,0,50), 100);
  //  Ring2.RainbowCycle(3);
  //  Ring2.Color1 = Ring1.Color1;
    //Stick.Scanner(Ring1.Color(255,0,0), 25);
    //Stick.Scanner(Stick.Color(255,0,0), 25);
    Stick.RainbowCycle(10);
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  
  // Include the BLE UART (AKA 'NUS') 128-bit UUID
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
    //neopattern update the stick (strip in this case)
    Stick.Update();

    if (Stick.nextPattern == true && Stick.LoopMode == true) {
     // Stick.Update();
      Stick.nextPattern = false;
      int rando = random(1,6);
      Serial.println(rando);
      startShowLoop(rando);
      }
      else {
      //  Stick.Update();
  // Wait for new data to arrive
  uint8_t len = readPacket(&bleuart, 10);
  if (len == 0) return;

  // Got a packet!
  // printHex(packetbuffer, len);

  // Buttons
  if (packetbuffer[1] == 'B') {
    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';
    Serial.print ("Button "); Serial.print(buttnum);
    if (pressed) {
      Serial.println(" pressed");
      Serial.println(Stick.LoopMode);
      startShow(buttnum);
    } else {
      Serial.println(" released");
    } 
  }
 }
}

// neo pattern completion routines
//------------------------------------------------------------
//Completion Routines - get called on completion of a pattern
//------------------------------------------------------------
 
// Ring1 Completion Callback
/*void Ring1Complete()
{
    if (digitalRead(9) == LOW)  // Button #2 pressed
    {
        // Alternate color-wipe patterns with Ring2
        Ring2.Interval = 40;
        Ring1.Color1 = Ring1.Wheel(random(255));
        Ring1.Interval = 20000;
    }
    else  // Retrn to normal
    {
      Ring1.Reverse();
    }
}
 */
// Ring 2 Completion Callback
/*
void Ring2Complete()
{
    if (digitalRead(9) == LOW)  // Button #2 pressed
    {
        // Alternate color-wipe patterns with Ring1
        Ring1.Interval = 20;
        Ring2.Color1 = Ring2.Wheel(random(255));
        Ring2.Interval = 20000;
    }
    else  // Retrn to normal
    {
        Ring2.RainbowCycle(random(0,10));
    }
}
 */
// Stick Completion Callback
void StickComplete()
{
  uint32_t tempcolor = 0;
    // Random color change for next scan
   // Stick.Color1 = Stick.Wheel(random(255));
   //Stick.Reverse();
  switch(Stick.ActivePattern){
                case RAINBOW_CYCLE:
                    //Stick.Reverse();
                    break;
                case THEATER_CHASE:
                    //Stick.Reverse();
                    break;
                case COLOR_WIPE:
                   // tempcolor = Stick.Color1;
                   // Stick.Color1 = Stick.Color2;
                   // Stick.Reverse();
                    //Stick.Color2 = Stick.Color1;
                    break;
                case SCANNER:
                    break;
                case FADE:
                Stick.Reverse();
                    break;
  }
}

// from buttoncycler sketch
void startShow(int i) {
  switch(i){
    case 1: Stick.Fade(Stick.Color(255,0,0),Stick.Color(0,255,0),30,100);
            Stick.LoopMode = false;
            break;
    case 2: Stick.RainbowCycle(10);
            Stick.LoopMode = false;
            break;
    case 3: Stick.TheaterChase(Stick.Color(0,255,0),Stick.Color(0,0,255), 100);
            Stick.LoopMode = false;
            break;
    case 4: Stick.TheaterChase(Stick.Color(255,51,255),Stick.Color(0,0,0), 100);
            Stick.LoopMode = false;
            break;
    case 5: Stick.ColorWipe(Stick.Color(255,0,0), Stick.Color(0,255,0), 100);
            Stick.LoopMode = false;
            break;
    case 6: Stick.Scanner(Stick.Color(255,0,0), Stick.Color(0,255,0), 50);
            Stick.LoopMode = false;
            break;
    case 8: Stick.LoopMode = true;
    Serial.println(Stick.LoopMode);
            break;
  }
}

void startShowLoop(int i) {
  switch(i){
    case 1: Stick.Fade(Stick.Color(255,0,0),Stick.Color(0,255,0),30,100);
            break;
    case 2: Stick.RainbowCycle(10);
            break;
    case 3: Stick.TheaterChase(Stick.Color(0,255,0),Stick.Color(0,0,255), 100);
            break;
    case 4: Stick.TheaterChase(Stick.Color(255,51,255),Stick.Color(0,0,0), 100);
            break;
    case 5: Stick.ColorWipe(Stick.Color(255,0,0), Stick.Color(0,255,0), 100);
            break;
    case 6: Stick.Scanner(Stick.Color(255,0,0), Stick.Color(0,255,0), 50);
            break;
    case 8: Stick.LoopMode = true;
    Serial.println(Stick.LoopMode);
            break;
  }
}
