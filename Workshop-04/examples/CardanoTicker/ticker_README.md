# Scrolling Ticker

The scrolling ticker at the bottom of the screen continuously scrolls token prices horizontally. This creates a stock-market-style ticker effect!

## What It Displays

For each token, the ticker shows:
- **Ticker symbol** in larger text (e.g., "MIN")
- **Price per token** in USD (e.g., "$0.0123")
- **24h change** color-coded green (up) or red (down) (e.g., "+5.67%")

## How It Works

The ticker uses a sprite (off-screen buffer) to reduce flicker. Here's the clever trick:

1. All token information is drawn into a sprite buffer (not directly to the screen)
2. The content is drawn twice, side by side, to create seamless looping
3. The sprite scrolls left, and when the first copy scrolls off, the second copy is already visible
4. When we reach the end, we reset to the beginning - creating an endless scroll effect!

The ticker updates every 30 milliseconds to create smooth scrolling. The scroll speed is 2 pixels per update, which creates a nice, readable scrolling speed.

## Understanding Sprites

A sprite is an off-screen buffer (like a canvas) that you draw to, then push to the screen all at once. This reduces flicker because the screen updates in one operation instead of many small updates. Think of it like drawing on paper first, then showing the whole paper at once, rather than drawing directly on a whiteboard where people can see each line being drawn.

## Key Functions

- `initTicker()`: Initializes the ticker display (call once in setup)
  - Sets up the sprite buffer
  - Calculates total content width
- `updateTicker()`: Updates the ticker display (call repeatedly in loop)
  - Draws content to sprite buffer
  - Scrolls the content left
  - Pushes sprite to display
- `calculateContentWidth()`: Calculates the total width of all token content
- `drawContentLine(xPos)`: Draws all token information at a given horizontal position
- `getTokenPrice(token)`: Calculates price per token (value / amount)

## Code Structure

### Initialization (`initTicker()`)
1. Fill screen with black
2. Configure sprite with 16-bit color depth
3. Create sprite buffer (full screen width × ticker height)
4. Calculate total content width for seamless looping

### Update Loop (`updateTicker()`)
1. Clear sprite buffer with black
2. Draw first copy of content at current scroll position
3. Draw second copy offset by content width (for seamless loop)
4. Push sprite to display at bottom of screen
5. Update scroll position (move left by scrollSpeed pixels)
6. Reset scroll position when content has scrolled completely

## Seamless Looping

The seamless loop effect is achieved by:
- Drawing content twice, side by side
- When first copy scrolls off left, second copy is already visible
- When scroll position reaches content width, reset to 0
- This creates an endless scrolling effect!

## Scroll Speed

- **Scroll speed**: 2 pixels per update
- **Update interval**: 30 milliseconds (~33 updates per second)
- This creates smooth, readable scrolling

## Price Calculation

The price per token is calculated as:
```
price = token.value / token.amount
```

This gives the price per individual token in USD. If the amount is 0, the price is 0 (to avoid division by zero).

## Color Coding

The 24h change uses color coding:
- **Green** (`TFT_GREEN`): Positive change (price went up)
- **Red** (`TFT_RED`): Negative change (price went down)

This makes it easy to see at a glance which tokens are performing well!





