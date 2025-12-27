# Token Screen

The token screen displays all your token holdings in a table format. Each row shows one token with its ticker symbol, amount you own, total value, and 24-hour price change.

## What It Shows

- **Ticker**: The token symbol (e.g., "MIN", "HOSKY", "ADA")
- **Amount**: How many tokens you own
- **Value**: Total value in USD (amount × price per token)
- **24h Change**: Price change percentage, color-coded green (up) or red (down)

## How It Works

The screen loops through all tokens using `getTokenCount()` and `getToken(i)` functions from the data fetcher. For each token, it draws a row in the table. The screen can display up to 8 tokens (limited by screen space).

The 24h change is color-coded: green for positive changes (price went up) and red for negative changes (price went down). This makes it easy to see at a glance which tokens are performing well!

The function first draws column headers, then loops through each token to display its ticker, amount, value, and 24h change. Long token names are truncated to fit on screen. The function includes a safety check to stop drawing if running out of screen space.

## Key Functions

- `drawTokenScreen()`: Main function that renders the entire token screen
- `getTokenCount()`: Returns the number of tokens available
- `getToken(i)`: Retrieves token data at index i from the data fetcher
- `renderHeader()`: Draws the screen header with title and page indicator
- `clearContentArea()`: Clears the content area below the header

## Code Structure

The screen follows a table layout:
1. Draw header with "Token Positions" title and page indicator (1)
2. Clear content area
3. Display screen title with token count (e.g., "Tokens(5)")
4. Draw column headers: Ticker, Amount, Value, 24h Change
5. Loop through each token and draw a row with:
   - Token ticker (truncated if too long)
   - Amount owned
   - Total value in USD
   - 24h change (color-coded)
6. If there are more tokens than can be displayed, show "... and X more"

## Color Coding

The 24h change uses color coding for quick visual feedback:
- **Green**: Positive change (price went up) - `TFT_GREEN`
- **Red**: Negative change (price went down) - `TFT_RED`

This makes it easy to see at a glance which tokens are performing well!






