/**
 * token_screen.h - Header file for token display screen
 * 
 * This file declares the function to draw the token screen, which displays
 * information about the Cardano tokens in your wallet, including token names,
 * quantities, values, and 24-hour price changes.
 */

#ifndef TOKEN_SCREEN_H
#define TOKEN_SCREEN_H

/**
 * Draw the token screen
 * 
 * Displays your Cardano token holdings with their values and price changes.
 * This screen is part of the rotating display cycle.
 */
void drawTokenScreen();

#endif

