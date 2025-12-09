/**
 * status_screen.h - Header file for status display screen
 * 
 * This file declares the function to draw the status screen, which displays
 * system information such as WiFi connection status, last update times,
 * and other diagnostic information.
 */

#ifndef STATUS_SCREEN_H
#define STATUS_SCREEN_H

/**
 * Draw the status screen
 * 
 * Displays system status information including network connectivity,
 * API fetch times, and other diagnostic data.
 * This screen is part of the rotating display cycle.
 */
void drawStatusScreen();

#endif

