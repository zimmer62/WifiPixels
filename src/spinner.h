#ifndef SPINNER_H
#define SPINNER_H

#include <ESP8266WiFi.h>
#include "pixel_helper.h"

void DoSpinMode(PIXEL_HELPER_CLASS*);
void DoBackSpinMode(PIXEL_HELPER_CLASS*);
void DoPingPongSpinMode(PIXEL_HELPER_CLASS*);
void ParseSpin(String, PIXEL_HELPER_CLASS*);
void ParseUpdate(String, PIXEL_HELPER_CLASS*);

#endif
