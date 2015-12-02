#ifndef PIXEL_HELPER_H
#define PIXEL_HELPER_H

#include <ESP8266WiFi.h>
#include <NeoPixelBus.h>

#define RGBMode_Blank 0
#define RGBMode_BLEND 1
#define CustomPattern_Mode 2
#define Spin_Mode 3


#define pixelPin 2
#define pixelCount 16

class PIXEL_HELPER_CLASS
{
  public:
    PIXEL_HELPER_CLASS();
    void pixelLoop();
    void ProcessCommand(String command);
    int LEDMode = RGBMode_Blank;
    bool ProcessSerial = false;

    RgbColor RGBStringToRGB(String input);
    void SetAll(RgbColor colour);
    
    NeoPixelBus strip = NeoPixelBus(pixelCount, pixelPin);
	NeoPixelAnimator animations = NeoPixelAnimator(&strip);
    long previousMillis = 0;
};

extern PIXEL_HELPER_CLASS * pixel_helper;

#endif
