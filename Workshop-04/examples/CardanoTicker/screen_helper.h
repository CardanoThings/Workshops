#ifndef SCREEN_COMMON_H
#define SCREEN_COMMON_H

#include <Arduino.h>

void renderHeader(const char *title, uint8_t activeIndex, uint8_t totalScreens);
void clearContentArea(); // Clears the area between header and ticker

#endif
