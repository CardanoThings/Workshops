/**
 * ticker.h - Header file for scrolling ticker
 * 
 * This file declares the functions for the scrolling token price ticker
 * that appears at the bottom of the screen.
 */

#ifndef TICKER_H
#define TICKER_H

#include <Arduino.h>

/**
 * Initialize the ticker display
 * 
 * Sets up the sprite buffer and calculates content width.
 * Call this once in setup().
 */
void initTicker();

/**
 * Update the ticker display
 * 
 * Draws the scrolling ticker and updates the scroll position.
 * Call this repeatedly in loop() to create smooth scrolling animation.
 */
void updateTicker();

#endif
