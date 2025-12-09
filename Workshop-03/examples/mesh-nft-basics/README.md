# Mesh NFT Basics

This directory contains two fundamental examples for working with NFTs on Cardano using Mesh SDK:

1. **mint-nft.js** - Mint your first NFT on the Cardano blockchain
2. **burn-nft.js** - Permanently destroy (burn) an NFT

Both examples demonstrate core NFT operations using Mesh SDK with the Koios provider on the Cardano Preprod testnet.

## Overview

These examples demonstrate the core concepts of working with NFTs on Cardano:
- Creating and minting NFTs with metadata
- Burning NFTs permanently
- Understanding Policy IDs and Token Names
- Working with transaction building and signing

## Requirements

- **Node.js** (v14 or higher recommended)
- **npm** (Node Package Manager)
- **@meshsdk/core** - Mesh SDK for Cardano
- A Cardano testnet wallet with mnemonic phrase
- Some tADA (testnet ADA) for transaction fees
- A code editor (VS Code, Cursor, etc.)

## Installation

### Step 1: Install Node.js

If you don't have Node.js installed:

1. Visit [nodejs.org](https://nodejs.org/)
2. Download and install the LTS (Long Term Support) version
3. Verify installation by running: `node --version` and `npm --version`

### Step 2: Create Project Directory

```bash
mkdir mesh-nft-basics
cd mesh-nft-basics
```

### Step 3: Initialize npm Project

```bash
npm init -y
```

### Step 4: Install Dependencies

```bash
npm install @meshsdk/core
```

### Step 5: Configure Your Wallet

**IMPORTANT:** Never hardcode your mnemonic in production code! For these examples, replace the mnemonic array in the code files with your testnet wallet mnemonic. In production, always use environment variables:

```javascript
const mnemonic = process.env.WALLET_MNEMONIC?.split(' ') || [];
```

## Example 1: Mint Your First NFT

### Overview

This example demonstrates how to mint your first NFT on the Cardano blockchain using Mesh SDK. It walks through each step of the process, from setting up your wallet to submitting the transaction.

### Step-by-Step Process

1. **Set up the Provider:** Connect to the Cardano network (testnet for testing, mainnet for real NFTs)
2. **Create Your Wallet:** Initialize your wallet using your mnemonic phrase
3. **Initialize Wallet:** Load your wallet information from the blockchain
4. **Get UTXOs:** Retrieve your available funds (needed to pay transaction fees)
5. **Get Change Address:** Your wallet address where leftover funds are returned
6. **Create Forging Script:** Define the policy that controls who can mint NFTs
7. **Prepare Metadata:** Create the data that will be stored in your NFT
8. **Generate Policy ID:** Create a unique identifier for your NFT collection
9. **Create Token Name:** Give your NFT a unique name
10. **Convert to Hex:** Convert the name to hexadecimal format (blockchain requirement)
11. **Structure Metadata:** Format metadata according to CIP-25 standard
12. **Build Transaction:** Create the transaction that will mint your NFT
13. **Sign Transaction:** Sign it with your wallet to authorize it
14. **Submit Transaction:** Send it to the Cardano network
15. **View Results:** Get the transaction hash to view on the explorer

### Key Concepts

- **Policy ID:** A unique identifier for your NFT collection. All NFTs with the same Policy ID belong to the same collection.
- **Token Name:** A unique name for each individual NFT within a collection.
- **UTXOs:** Unspent Transaction Outputs - these are like coins in your wallet that you can spend.
- **Transaction Hash:** A unique identifier for your transaction. You can use this to view it on blockchain explorers.
- **Minting:** The process of creating a new NFT on the blockchain.

### Important Notes

- **Replace the mnemonic:** The example uses placeholder words. Replace them with your actual wallet mnemonic.
- **Use environment variables:** In production, never hardcode your mnemonic. Use `process.env.WALLET_MNEMONIC` instead.
- **Testnet vs Mainnet:** The code uses 'preprod' (testnet) which is free. Change to 'api' for mainnet (real money).
- **Transaction Fees:** You need tADA (testnet ADA) in your wallet to pay for transaction fees (about 0.2 tADA per NFT).
- **Network ID:** 0 = testnet, 1 = mainnet. Make sure this matches your provider setting.

### Running the Script

1. Make sure you have `@meshsdk/core` installed: `npm install @meshsdk/core`
2. Replace the mnemonic array in `mint-nft.js` with your actual testnet wallet mnemonic
3. Make sure your wallet has some tADA for transaction fees
4. Run the script: `node mint-nft.js`
5. You should see the Policy ID and transaction hash printed to the console
6. Use the Cardano Explorer link to view your transaction and NFT

## Example 2: Burn an NFT

### Overview

This code permanently destroys (burns) an NFT from your wallet. Burning an NFT is like minting a negative amount - you're removing it from the blockchain forever. Think of it as the opposite of minting.

### How It Works

1. **Set up the Provider:** Connect to the Cardano network (same network where you minted the NFT)
2. **Create Your Wallet:** Initialize your wallet using your mnemonic phrase (must be the same wallet that owns the NFT)
3. **Initialize Wallet:** Load your wallet information from the blockchain
4. **Get UTXOs:** Retrieve your available funds (needed to pay transaction fees)
5. **Get Change Address:** Your wallet address where leftover funds are returned
6. **Create Forging Script:** Recreate the same policy script used when minting (must match exactly)
7. **Generate Policy ID:** Calculate the Policy ID from the forging script (must match the minting Policy ID)
8. **Set Token Name:** Specify the exact token name of the NFT you want to burn
9. **Convert to Hex:** Convert the token name to hexadecimal format (blockchain requirement)
10. **Build Transaction:** Create a transaction that mints "-1" of the token (this burns 1 token)
11. **Sign Transaction:** Sign it with your wallet to authorize it
12. **Submit Transaction:** Send it to the Cardano network
13. **View Results:** Get the transaction hash to view on the explorer

### Key Concepts

- **Burning = Minting -1:** To burn a token, you mint a negative amount. Minting "-1" destroys one token.
- **Policy ID Must Match:** You must use the exact same Policy ID that was used when minting the NFT.
- **Token Name Must Match:** The token name must be exactly the same as when you minted it (case-sensitive).
- **You Must Own It:** You can only burn NFTs that are currently in your wallet.
- **Permanent Action:** Once burned, the NFT is permanently destroyed and cannot be recovered.

### Important Notes

- **Replace the mnemonic:** The example uses placeholder words. Replace them with your actual wallet mnemonic.
- **Set the token name:** You must set the `tokenName` variable to the exact name of the NFT you want to burn.
- **Same wallet required:** You must use the same wallet that owns the NFT you want to burn.
- **Transaction fees:** You still pay a transaction fee to burn an NFT (about 0.2 tADA on testnet).
- **Cannot be undone:** Burning is permanent. Make sure you really want to destroy the NFT before running this code.

### Running the Script

1. Make sure you have `@meshsdk/core` installed: `npm install @meshsdk/core`
2. Replace the mnemonic array in `burn-nft.js` with your actual testnet wallet mnemonic
3. Set the `tokenName` variable to the exact name of the NFT you want to burn (this should match the tokenName from when you minted it)
4. Make sure your wallet has some tADA for transaction fees
5. Run the script: `node burn-nft.js`
6. You should see the transaction hash printed to the console
7. Use the Cardano Explorer link to verify the NFT has been burned

### How to Find Your Token Name

When you mint an NFT, the server returns a `tokenName` in the response. You can use that exact value to burn the NFT. Alternatively, you can check your wallet or the blockchain explorer to find the token name.

**From API Response:** When you mint an NFT via the API server, the response includes a `tokenName` field. Copy that value exactly.

**From Wallet:** Open your Cardano wallet (Yoroi, Vespr, Eternl, etc.) and view your NFTs. The token name is usually displayed in the NFT details.

**From Blockchain Explorer:** Search for your Policy ID on CardanoScan or another explorer, and you'll see all NFTs in the collection with their token names.

## Security Notes

- Always store your mnemonic in environment variables, never in code
- Use testnet wallets for development and testing
- Never commit files containing mnemonics to version control
- Always double-check the token name before burning an NFT
- Burning is permanent - make sure you really want to destroy the NFT

