#ifndef SCREEN_COMMON_H
#define SCREEN_COMMON_H

#include <Arduino.h>

extern const uint8_t TOTAL_SCREENS;

void renderHeader(const char *title, uint8_t activeIndex);
void clearContentArea(); // Clears the area between header and ticker

#endif
