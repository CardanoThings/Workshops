# NFT Screen

The NFT screen displays all your NFT collections. Each row shows one collection with its name, how many NFTs you own from that collection, and the floor price.

## What It Shows

- **Name**: The NFT collection name (e.g., "Cardano Punks", "SpaceBudz")
- **Amount**: How many NFTs you own from this collection (if you own 3 NFTs from the same collection, it shows "3")
- **Floor Price**: The lowest current selling price for this collection in ADA

## How It Works

NFTs are grouped by Policy ID (collection identifier). If you own multiple NFTs from the same collection, they're shown as one row with the total count. The floor price comes from the Cexplorer API and shows the cheapest NFT from that collection currently for sale.

The screen uses `getNftCount()` and `getNFT(i)` functions from the data fetcher, just like the token screen!

The function displays column headers for Name, Amount, and Floor Price. For each NFT collection, it shows the collection name (truncated if too long), the number of NFTs owned, and the floor price in ADA. If floor price data isn't available yet, it displays "N/A".

## Key Functions

- `drawNFTScreen()`: Main function that renders the entire NFT screen
- `getNftCount()`: Returns the number of NFT collections available
- `getNFT(i)`: Retrieves NFT collection data at index i from the data fetcher
- `renderHeader()`: Draws the screen header with title and page indicator
- `clearContentArea()`: Clears the content area below the header

## Code Structure

The screen follows a table layout:
1. Draw header with "NFT Positions" title and page indicator (2)
2. Clear content area
3. Display screen title with NFT count (e.g., "NFTs(3)")
4. Draw column headers: Name, Amount, Floor Price
5. Loop through each NFT collection and draw a row with:
   - Collection name (truncated if too long)
   - Number of NFTs owned (as integer, no decimals)
   - Floor price in ADA (or "N/A" if not available)
6. If there are more collections than can be displayed, show "... and X more"

## NFT Grouping

NFTs are grouped by Policy ID (collection identifier). This means:
- If you own 3 NFTs from the same collection, they appear as one row showing "3"
- Each unique collection appears as a separate row
- The floor price represents the cheapest NFT from that collection currently for sale

## Floor Price

The floor price comes from the Cexplorer API and represents the lowest current selling price for NFTs in that collection. If the floor price data hasn't been fetched yet (or isn't available), the screen displays "N/A" instead.

