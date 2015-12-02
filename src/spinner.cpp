
#include <ESP8266WiFi.h>
#include "BlendRGB.h"
#include "pixel_helper.h"

struct RGBBLEND {
	RgbColor RGB1;    // Start Color
	RgbColor RGB2;    // End Color
	int Cycles;       // Blend Cycles. 0=continuous
	int Interval;     // Delay between each loop cycle
	int CycleNumber;  // Current cycle // TO-DO
	float Direction;    // RGB1->RGB2 or RGB2->RGB1
	float Progress;     // 0-255 0=RGB1 255=RGB2 
	int ProgressSteps; // Rate of progress // TO-DO 
};

RGBBLEND SpinModeSettings;

void ParseSpin(String input, PIXEL_HELPER_CLASS* p_helper) {
	p_helper->LEDMode = Spin_Mode;

	input.remove(0, 8);
	SpinModeSettings.RGB1 = p_helper->RGBStringToRGB(input.substring(0, input.indexOf(',')));

	input.remove(0, input.indexOf(',') + 1);
	SpinModeSettings.RGB2 = p_helper->RGBStringToRGB(input.substring(0, input.indexOf(',')));

	input.remove(0, input.indexOf(',') + 1);
	SpinModeSettings.Cycles = input.substring(0, input.indexOf(',')).toInt();

	input.remove(0, input.indexOf(',') + 1);
	SpinModeSettings.Interval = input.toInt();

	SpinModeSettings.CycleNumber = 0;
	SpinModeSettings.Progress = 0;
	SpinModeSettings.Direction = 1;
	p_helper->previousMillis = millis() - SpinModeSettings.Interval; //todo: what that heck is this line doing and why?

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


void DoSpinMode(PIXEL_HELPER_CLASS* p_helper) {
	unsigned long currentMillis = millis();

	if (currentMillis - p_helper->previousMillis > SpinModeSettings.Interval) {
		p_helper->previousMillis = currentMillis;

		// Step Progress and control direction on Min and max
		SpinModeSettings.Progress = SpinModeSettings.Progress + SpinModeSettings.Direction;

		if (SpinModeSettings.Progress >= 1) {
			SpinModeSettings.Progress = 1.0;
			SpinModeSettings.Direction = -0.003921568627451;
		}
		if (SpinModeSettings.Progress <= 0) {
			SpinModeSettings.Progress = 0.0;
			SpinModeSettings.Direction = 0.003921568627451;
		}

		RgbColor rgb;
		rgb = rgb.LinearBlend(SpinModeSettings.RGB1, SpinModeSettings.RGB2, SpinModeSettings.Progress);

		/*if (rgb.R > 20 || rgb.B > 20 || rgb.G > 20) {
			Serial.print("Progress=");
			Serial.println(SpinModeSettings.Progress);
			Serial.print("Direction=");
			Serial.println(SpinModeSettings.Direction);
			Serial.print("currentMillis=");
			Serial.println(currentMillis);
			Serial.print("previousMillis=");
			Serial.println(p_helper->previousMillis);
			Serial.print("Interval=");
			Serial.println(SpinModeSettings.Interval);

			Serial.print("RGB=");
			Serial.print(rgb.R);
			Serial.print(".");
			Serial.print(rgb.G);
			Serial.print(".");
			Serial.println(rgb.B);
			Serial.println("---");
		}*/



		p_helper->SetAll(rgb);

		



		//strip.SetPixelColor(K, colour);
		if (p_helper->strip.CanShow()) p_helper->strip.Show();
	}

}
//strip.SetPixelColor(0, RgbColor(25, 0, 0));
//strip.SetPixelColor(1, RgbColor(0, 25, 0));
//strip.SetPixelColor(2, RgbColor(0, 0, 25));
//strip.SetPixelColor(3, RgbColor(25, 25, 25));
//strip.Show();
//Serial.println("3");
//delay(1000);
//Serial.println("2");
//delay(1000);
//Serial.println("1");
//delay(1000);
//strip.SetPixelColor(0, RgbColor(0));
//strip.SetPixelColor(1, RgbColor(0));
//strip.SetPixelColor(2, RgbColor(0));
//strip.SetPixelColor(3, RgbColor(0));
//strip.Show();
//Serial.println("Off"); */
