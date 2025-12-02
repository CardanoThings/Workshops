/**
 * screen_helper.h - Header file for screen rendering utilities
 * 
 * This file provides shared functions and constants for drawing screens.
 * All screens use these functions to maintain consistent layout and styling.
 * 
 * Screen Layout:
 * - Header (top): Shows screen title and page indicators (dots)
 * - Content (middle): Shows the actual data (wallet, tokens, NFTs, etc.)
 * - Ticker (bottom): Shows scrolling token prices
 */

#ifndef SCREEN_COMMON_H
#define SCREEN_COMMON_H

#include <Arduino.h>

// Total number of screens in the rotation
extern const uint8_t TOTAL_SCREENS;

// Shared constants for screen layout (in pixels)
// These ensure all screens have consistent spacing
constexpr int kHeaderHeight = 34;      // Height of header bar at top
constexpr int kTickerHeight = 30;      // Height of scrolling ticker at bottom
                                       // Must match scrollAreaHeight in ticker.cpp
constexpr int MAX_DISPLAY_ITEMS = 8;   // Maximum items to display on screen
                                       // Limited by screen size and readability

/**
 * Render the header bar at the top of the screen
 * 
 * The header shows:
 * - Screen title (left side)
 * - Page indicators (right side) - dots showing which screen you're on
 * 
 * @param title The text to display as the screen title
 * @param activeIndex Which screen is currently active (0-3)
 *                    Used to highlight the correct page indicator dot
 */
void renderHeader(const char *title, uint8_t activeIndex);

/**
 * Clear the content area between header and ticker
 * 
 * This erases the middle section of the screen so new content can be drawn.
 * Think of it like erasing a whiteboard before writing new information.
 */
void clearContentArea();

#endif
