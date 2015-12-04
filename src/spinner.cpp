
#include <ESP8266WiFi.h>
#include "BlendRGB.h"
#include "pixel_helper.h"


struct RGBBLEND {
	PIXEL_HELPER_CLASS* p_helper;
	RgbColor RGB1;    // Start Color
	RgbColor RGB2;    // End Color
	int Cycles;       // Blend Cycles. 0=continuous
	int Interval;     // Delay between each loop cycle
	int FadeRate;     // Cycles to Fade
	int CycleNumber;  // Current cycle // TO-DO
	int Direction;    // RGB1->RGB2 or RGB2->RGB1
	float Progress;     // 0-255 0=RGB1 255=RGB2 
	int ProgressSteps; // Rate of progress // TO-DO 
};

RGBBLEND SpinModeSettings;

void ParseUpdate(String input, PIXEL_HELPER_CLASS* p_helper) {
	input.remove(0, 7);
	SpinModeSettings.RGB1 = p_helper->RGBStringToRGB(input.substring(0, input.indexOf(',')));

	input.remove(0, input.indexOf(',') + 1);
	SpinModeSettings.RGB2 = p_helper->RGBStringToRGB(input.substring(0, input.indexOf(',')));

	input.remove(0, input.indexOf(',') + 1);
	SpinModeSettings.Cycles = input.substring(0, input.indexOf(',')).toInt();

	input.remove(0, input.indexOf(',') + 1);
	SpinModeSettings.Interval = input.substring(0, input.indexOf(',')).toInt();

	input.remove(0, input.indexOf(',') + 1);
	SpinModeSettings.FadeRate = input.toInt();
}

void ParseSpin(String input, PIXEL_HELPER_CLASS* p_helper) {
	SpinModeSettings.p_helper = p_helper;
	p_helper->LEDMode = Spin_Mode;

	input.remove(0, 8);
	SpinModeSettings.RGB1 = p_helper->RGBStringToRGB(input.substring(0, input.indexOf(',')));

	input.remove(0, input.indexOf(',') + 1);
	SpinModeSettings.RGB2 = p_helper->RGBStringToRGB(input.substring(0, input.indexOf(',')));

	input.remove(0, input.indexOf(',') + 1);
	SpinModeSettings.Cycles = input.substring(0, input.indexOf(',')).toInt();

	input.remove(0, input.indexOf(',') + 1);
	SpinModeSettings.Interval = input.substring(0, input.indexOf(',')).toInt();

	input.remove(0, input.indexOf(',') + 1);
	SpinModeSettings.FadeRate = input.toInt();

	


	SpinModeSettings.CycleNumber = 0;
	SpinModeSettings.Progress = 0;
	SpinModeSettings.ProgressSteps = 0;
	SpinModeSettings.Direction = 1;
	SpinModeSettings.p_helper->previousMillis = millis() - SpinModeSettings.Interval; //todo: what that heck is this line doing and why?
	SpinModeSettings.p_helper->animations.FadeTo(SpinModeSettings.FadeRate, RgbColor(0, 0, 0));

	//block until all pixels are OFF
	int n = 0;
	for (int K = 0; K < pixelCount; K++) {
		RgbColor color = p_helper->strip.GetPixelColor(K);
		n = n + color.R + color.G + color.B;
	}
	while (n > 0) {
		n = 0;
		for (int K = 0; K < pixelCount; K++) {
			RgbColor color = p_helper->strip.GetPixelColor(K);
			n = n + color.R + color.G + color.B;
		}
		SpinModeSettings.p_helper->animations.UpdateAnimations(1000);
		if (SpinModeSettings.p_helper->strip.CanShow()) SpinModeSettings.p_helper->strip.Show();
		delay(1);
	}


}

AnimUpdateCallback animCurrentOneTwoOneOff = [=](uint16_t n, float progress)
{
	// progress will start at 0.0 and end at 1.0
	RgbColor currentColor = SpinModeSettings.p_helper->strip.GetPixelColor(n);
    RgbColor updatedColor;
	if (0.00 <= progress && progress <= 0.25) {
		updatedColor = RgbColor::LinearBlend(currentColor, SpinModeSettings.RGB1, progress*4.0);
	}
	if (0.25 < progress && progress <= 0.50) {
		updatedColor = RgbColor::LinearBlend(SpinModeSettings.RGB1, SpinModeSettings.RGB2, (progress-.25)*4.0);
	}
	if (0.50 < progress && progress <= 0.75) {
		updatedColor = RgbColor::LinearBlend(SpinModeSettings.RGB2, SpinModeSettings.RGB1, (progress-.50)*4.0);
	}
	if (0.75 < progress && progress <= 1.00) {
		updatedColor = RgbColor::LinearBlend(SpinModeSettings.RGB1, RgbColor(0, 0, 0), (progress-.75)*4.0);
	}
	SpinModeSettings.p_helper->strip.SetPixelColor(n, updatedColor);
};

AnimUpdateCallback animCurrentOneTwoOff = [=](uint16_t n, float progress)
{
	// progress will start at 0.0 and end at 1.0
	/*RgbColor currentColor = SpinModeSettings.p_helper->strip.GetPixelColor(n);
	Serial.print(n);
	Serial.print("-R:");
	Serial.print(currentColor.R);
	Serial.print("G:");
	Serial.print(currentColor.G);
	Serial.print("B:");
	Serial.println (currentColor.B);*/
	

	RgbColor updatedColor;
	if (0.00 <= progress && progress <= 0.33) {
		updatedColor = RgbColor::LinearBlend(RgbColor(0, 0, 0), SpinModeSettings.RGB1, progress*3.0);
	}
	if (0.33 < progress && progress <= 0.66) {
		updatedColor = RgbColor::LinearBlend(SpinModeSettings.RGB1, SpinModeSettings.RGB2, (progress - .33)*3.0);
	}
	if (0.66 < progress && progress <= 1.0) {
		updatedColor = RgbColor::LinearBlend(SpinModeSettings.RGB2, RgbColor(0, 0, 0), (progress - .67)*3+0.01);
	}	
	SpinModeSettings.p_helper->strip.SetPixelColor(n, updatedColor);
};


//SPINNER,25.0.0,0.25.25,0,55,1250
//SPINNER,25.0.0,0.0.25,0,50,100
void DoSpinMode(PIXEL_HELPER_CLASS* p_helper) {	
	
	unsigned long currentMillis = millis();

	if (currentMillis - p_helper->previousMillis > SpinModeSettings.Interval) {
		p_helper->previousMillis = currentMillis;
		p_helper->animations.StartAnimation(SpinModeSettings.ProgressSteps, SpinModeSettings.FadeRate, animCurrentOneTwoOff);
		SpinModeSettings.ProgressSteps = SpinModeSettings.ProgressSteps + 1;
		if (SpinModeSettings.ProgressSteps == pixelCount) SpinModeSettings.ProgressSteps = 0;
	}

	p_helper->animations.UpdateAnimations(100);
	if (p_helper->strip.CanShow()) p_helper->strip.Show();
	//delay(1);
}

void DoBackSpinMode(PIXEL_HELPER_CLASS* p_helper) {

	unsigned long currentMillis = millis();

	if (currentMillis - p_helper->previousMillis > SpinModeSettings.Interval) {
		p_helper->previousMillis = currentMillis;
		p_helper->animations.StartAnimation(SpinModeSettings.ProgressSteps, SpinModeSettings.FadeRate, animCurrentOneTwoOff);
		SpinModeSettings.ProgressSteps = SpinModeSettings.ProgressSteps - 1;
		if (SpinModeSettings.ProgressSteps == -1) SpinModeSettings.ProgressSteps = pixelCount-1;
	}

	p_helper->animations.UpdateAnimations(100);
	if (p_helper->strip.CanShow()) p_helper->strip.Show();
	//delay(1);
}

void DoPingPongSpinMode(PIXEL_HELPER_CLASS* p_helper) {
	if (SpinModeSettings.ProgressSteps == 0) SpinModeSettings.Direction = 1;
	unsigned long currentMillis = millis();

	if (currentMillis - p_helper->previousMillis > SpinModeSettings.Interval) {
		p_helper->previousMillis = currentMillis;
		p_helper->animations.StartAnimation(SpinModeSettings.ProgressSteps, SpinModeSettings.FadeRate, animCurrentOneTwoOff);
		SpinModeSettings.ProgressSteps = SpinModeSettings.ProgressSteps + SpinModeSettings.Direction;
		if (SpinModeSettings.ProgressSteps == pixelCount) {
			SpinModeSettings.Direction = -1;
			SpinModeSettings.ProgressSteps + SpinModeSettings.Direction;
			SpinModeSettings.ProgressSteps + SpinModeSettings.Direction;
			SpinModeSettings.ProgressSteps + SpinModeSettings.Direction;
		}
	}

	p_helper->animations.UpdateAnimations(100);
	if (p_helper->strip.CanShow()) p_helper->strip.Show();
	//delay(1);
}



