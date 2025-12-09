# Wallet Screen

The wallet screen displays your Cardano wallet balance prominently. This is the first screen shown (index 0) in the rotating display.

## What It Shows

- **Balance**: Your ADA balance in large text (size 3) - this is the most important information!
- **Stake Address**: Your stake address, truncated to fit on screen (shows first 12 characters + "..." + last 12 characters)
- **Last Updated**: How long ago the balance was fetched (e.g., "2m 30s ago" or "just now")

## How It Works

The screen uses the `getWalletBalance()` function from the data fetcher to get the current balance. The "Last updated" time is calculated using `millis()` - the same timing technique you learned in previous workshops!

The function first draws the header using `renderHeader()`, then clears the content area. It displays the balance in large text (size 3) for emphasis, followed by a truncated stake address and the last update time. The time formatting converts milliseconds to a human-readable format (e.g., "2m 30s ago" or "just now").

## Key Functions

- `drawWalletScreen()`: Main function that renders the entire wallet screen
- `getWalletBalance()`: Retrieves the current ADA balance from the data fetcher
- `getLastKoiosFetchTime()`: Gets the timestamp of when the balance was last fetched
- `renderHeader()`: Draws the screen header with title and page indicator
- `clearContentArea()`: Clears the content area below the header

## Code Structure

The screen follows a simple layout:
1. Draw header with "Wallet" title and page indicator (0)
2. Clear content area
3. Display "Balance" label
4. Display ADA balance in large text
5. Display truncated stake address
6. Display last update time with human-readable formatting

## Time Formatting

The time formatting logic converts milliseconds to a readable format:
- Less than 10 seconds: "just now"
- Less than 1 minute: "Xs ago" (e.g., "30s ago")
- 1 minute or more: "Xm Ys ago" (e.g., "2m 30s ago")

This uses the same `millis()` timing technique you've been using throughout the workshops!

