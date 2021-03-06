#include <NeoPixelBus.h>
#include "pixel_helper.h"
#include "BlendRGB.h"
#include "CustomPattern.h"
#include "spinner.h"

PIXEL_HELPER_CLASS * pixel_helper;

RgbColor PIXEL_HELPER_CLASS::RGBStringToRGB(String input) {
  int index = 0;
  int R = input.substring(0, input.indexOf('.')).toInt();

  index = input.indexOf('.');
  int G = input.substring(index + 1, input.indexOf('.', index + 1)).toInt();

  index = input.indexOf('.', index + 1);
  int B = input.substring(index + 1).toInt();

  return RgbColor( R , G , B );
}

void PIXEL_HELPER_CLASS::SetAll(RgbColor colour) {
  for (int K = 0; K < pixelCount; K++) {
    strip.SetPixelColor(K, colour);
  }
}

PIXEL_HELPER_CLASS::PIXEL_HELPER_CLASS() {
  // Clear LEDS
  strip.Begin();
  strip.Show();
  
  // Turn onboard led off
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
}





// RGBBLEND,0.0.0,0.20.0,0,3
void PIXEL_HELPER_CLASS::ProcessCommand(String input) {
	AttractMode = RGBMode_Blank;

  Serial.print("ProcessCommand:");
  Serial.println(input);
	if (input.startsWith("RGBBLEND")) {
		ParseRGBBLEND(input,pixel_helper);
	}
	if (input.startsWith("CUSTOMPATTERN")) {
		ParseCustomPattern(input,pixel_helper);
	}
	if (input.startsWith("SPINNER")) {
		ParseSpin(input, pixel_helper);
	}
	if (input.startsWith("RENNIPS")) {
		ParseSpin(input, pixel_helper);
		LEDMode = Back_Spin_Mode;
	}
	if (input.startsWith("SPINIPS")) {
		ParseSpin(input, pixel_helper);
		LEDMode = Ping_Pong_Spin_Mode;
	}
	if (input.startsWith("UPDATE")) {
		ParseUpdate(input, pixel_helper);
	}

}

String SerialInput = "";

void PIXEL_HELPER_CLASS::DoAttractMode(PIXEL_HELPER_CLASS* p_helper) {
	//todo: insert some random stuff to make sure this gets changed up every so often.
	ParseRGBBLEND("RGBBLEND,25.0.0,0.25.25,0,10", pixel_helper);
	AttractMode = RGBMode_BLEND;
}

void PIXEL_HELPER_CLASS::pixelLoop() {
	if (LEDMode == RGBMode_BLEND || AttractMode == RGBMode_BLEND) {
		DoBlendMode(pixel_helper);
	}
	else if (LEDMode == CustomPattern_Mode || AttractMode == CustomPattern_Mode) {
		DoCustomPatternMode(pixel_helper);
	}
	else if (LEDMode == Spin_Mode || AttractMode == Spin_Mode) {
		DoSpinMode(pixel_helper);
	}
	else if (LEDMode == Back_Spin_Mode || AttractMode == Back_Spin_Mode) {
		DoBackSpinMode(pixel_helper);
	}
	else if (LEDMode == Ping_Pong_Spin_Mode || AttractMode == Ping_Pong_Spin_Mode) {
		DoPingPongSpinMode(pixel_helper);
	}
	else if (LEDMode == RGBMode_Blank || AttractMode == RGBMode_Blank) {
		DoAttractMode(pixel_helper);
	}

	if (AttractMode != RGBMode_Blank) {
		//DoAttractMode(pixel_helper);
	}

  
  if(ProcessSerial){
    while (Serial.available() > 0)
    {
        char recieved = Serial.read();
        SerialInput += recieved; 

        // Process message when new line character is recieved
        if (recieved == '\n')
        {
            ProcessCommand(SerialInput);
            SerialInput = "";
        }
    }
  }
 
}

