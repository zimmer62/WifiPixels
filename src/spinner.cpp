
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
	float Direction;    // RGB1->RGB2 or RGB2->RGB1
	float Progress;     // 0-255 0=RGB1 255=RGB2 
	int ProgressSteps; // Rate of progress // TO-DO 
};

RGBBLEND SpinModeSettings;

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

AnimUpdateCallback animOff = [=](uint16_t n, float progress)
{
	// progress will start at 0.0 and end at 1.0
	RgbColor updatedColor = RgbColor::LinearBlend(SpinModeSettings.RGB2, RgbColor(0, 0, 0), progress);
	SpinModeSettings.p_helper->strip.SetPixelColor(n, updatedColor);
};

AnimUpdateCallback animDown = [=](uint16_t n, float progress)
{
	// progress will start at 0.0 and end at 1.0
	RgbColor updatedColor = RgbColor::LinearBlend(SpinModeSettings.RGB2, SpinModeSettings.RGB1, progress);
	SpinModeSettings.p_helper->strip.SetPixelColor(n, updatedColor);
	Serial.print("down-");
	Serial.print(n);
	Serial.print("-");
	Serial.println(progress);
	if (progress == 1.00) {
		//TODO: probably not working for the same reason as below.
		SpinModeSettings.p_helper->animations.StartAnimation(n, SpinModeSettings.Interval, animOff);
	}
};

AnimUpdateCallback animUp = [=](uint16_t n, float progress)
{
	// progress will start at 0.0 and end at 1.0
	RgbColor updatedColor = RgbColor::LinearBlend(SpinModeSettings.RGB1, SpinModeSettings.RGB2, progress);
	SpinModeSettings.p_helper->strip.SetPixelColor(n, updatedColor);
	if (progress == 1.00) {
		/*Serial.print("upFinished-");
		Serial.print(n);
		Serial.print("-");
		Serial.println(progress);*/
		SpinModeSettings.p_helper->animations.StopAnimation(n);
		//TODO: why can't I start a new animation from this callback?
		SpinModeSettings.p_helper->animations.StartAnimation(n, SpinModeSettings.Interval, animDown);
	}
};


void DoSpinMode(PIXEL_HELPER_CLASS* p_helper) {	
	
	unsigned long currentMillis = millis();

	if (currentMillis - p_helper->previousMillis > SpinModeSettings.Interval) {
		p_helper->previousMillis = currentMillis;
		p_helper->animations.StartAnimation(SpinModeSettings.ProgressSteps, SpinModeSettings.FadeRate, animUp);
		SpinModeSettings.ProgressSteps = SpinModeSettings.ProgressSteps + 1;
		if (SpinModeSettings.ProgressSteps == 16) SpinModeSettings.ProgressSteps = 0;
	}

	p_helper->animations.UpdateAnimations(1000);
	if (p_helper->strip.CanShow()) p_helper->strip.Show();
	delay(1);
}




//uint16_t pixel = 0;
//int time = 500;
//int timedown = 250;
//
//RgbColor originalColor = strip.GetPixelColor(pixel);
//RgbColor color = RgbColor(0, 50, 50);
//
//// define the effect to apply, in this case linear blend
//AnimUpdateCallback animUpdate = [=](uint16_t n, float progress)
//{
//	// progress will start at 0.0 and end at 1.0
//	RgbColor updatedColor = RgbColor::LinearBlend(originalColor, color, progress);
//	strip.SetPixelColor(n, updatedColor);
//};
//
//AnimUpdateCallback animDown = [=](uint16_t n, float progress)
//{
//	// progress will start at 0.0 and end at 1.0
//	RgbColor updatedColor = RgbColor::LinearBlend(color, originalColor, progress);
//	strip.SetPixelColor(n, updatedColor);
//};
//
//NeoPixelAnimator animations(&strip);
//animations.StartAnimation(pixel, time, animUpdate);
//
//
//for (int K = 0; K < 1250; K++) {
//	animations.UpdateAnimations(1000);
//
//	if (K == 50) animations.StartAnimation(1, time, animUpdate);
//	if (K == 100) animations.StartAnimation(2, time, animUpdate);
//	if (K == 150) animations.StartAnimation(3, time, animUpdate);
//
//	if (K == 200) animations.StartAnimation(4, time, animUpdate);
//	if (K == 200) animations.StartAnimation(0, timedown, animDown);
//
//	if (K == 250) animations.StartAnimation(5, time, animUpdate);
//	if (K == 250) animations.StartAnimation(1, timedown, animDown);
//
//	if (K == 300) animations.StartAnimation(6, time, animUpdate);
//	if (K == 300) animations.StartAnimation(2, timedown, animDown);
//
//	if (K == 350) animations.StartAnimation(7, time, animUpdate);
//	if (K == 350) animations.StartAnimation(3, timedown, animDown);
//
//	if (K == 400) animations.StartAnimation(8, time, animUpdate);
//	if (K == 400) animations.StartAnimation(4, timedown, animDown);
//
//	if (K == 450) animations.StartAnimation(9, time, animUpdate);
//	if (K == 450) animations.StartAnimation(5, timedown, animDown);
//
//	if (K == 500) animations.StartAnimation(10, time, animUpdate);
//	if (K == 500) animations.StartAnimation(6, timedown, animDown);
//
//	if (K == 550) animations.StartAnimation(11, time, animUpdate);
//	if (K == 550) animations.StartAnimation(7, timedown, animDown);
//
//	if (K == 600) animations.StartAnimation(12, time, animUpdate);
//	if (K == 600) animations.StartAnimation(8, timedown, animDown);
//
//	if (K == 650) animations.StartAnimation(13, time, animUpdate);
//	if (K == 650) animations.StartAnimation(9, timedown, animDown);
//
//	if (K == 700) animations.StartAnimation(14, time, animUpdate);
//	if (K == 700) animations.StartAnimation(10, timedown, animDown);
//
//	if (K == 750) animations.StartAnimation(15, time, animUpdate);
//	if (K == 750) animations.StartAnimation(11, timedown, animDown);
//	if (K == 800) animations.StartAnimation(12, timedown, animDown);
//	if (K == 850) animations.StartAnimation(13, timedown, animDown);
//	if (K == 900) animations.StartAnimation(14, timedown, animDown);
//	if (K == 950) animations.StartAnimation(15, timedown, animDown);
//	strip.Show();
//	delay(1);
//}
