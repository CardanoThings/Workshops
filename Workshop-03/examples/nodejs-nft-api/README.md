# Complete API Server with NFT Minting

Node.js Express API server that receives sensor data from microcontrollers and automatically mints NFTs on the Cardano blockchain.

## Overview

This server receives sensor data from your microcontroller and turns it into an NFT on the Cardano blockchain. Think of it like a factory: it takes your sensor data, packages it as an NFT, and puts it on the blockchain forever.

## Requirements

- **Node.js** (v14 or higher recommended)
- **npm** (Node Package Manager)
- **Express.js** - Web framework for building APIs
- **CORS** - Middleware for enabling Cross-Origin Resource Sharing
- **@meshsdk/core** - Mesh SDK for Cardano blockchain interactions
- A Cardano testnet wallet with tADA (testnet ADA) for transaction fees
- A code editor (VS Code, Cursor, etc.)
- **Optional**: API testing tool like Insomnia or Postman

## Installation

### Step 1: Install Node.js

If you don't have Node.js installed:

1. Visit [nodejs.org](https://nodejs.org/)
2. Download and install the LTS (Long Term Support) version
3. Verify installation by running: `node --version` and `npm --version`

### Step 2: Create Project Directory

```bash
mkdir nodes-nft-api
cd nodes-nft-api
```

### Step 3: Initialize npm Project

```bash
npm init -y
```

### Step 4: Install Dependencies

```bash
npm install express cors @meshsdk/core
```

### Step 5: Update package.json

Make sure your `package.json` includes:

```json
{
  "type": "module"
}
```

This enables ES6 module syntax (import/export).

## Configuration

### Step 1: Set Up Your Wallet Mnemonic

**IMPORTANT**: Replace the placeholder mnemonic words with your actual wallet mnemonic phrase.

```javascript
const mnemonic = ["word1", "word2", "word3", "word4", "word5", "word6", "word7", "word8", "word9", "word10", "word11", "word12"];
```

**Security Best Practice**: In production, use environment variables instead:

```javascript
import dotenv from 'dotenv';
dotenv.config();

const mnemonic = process.env.WALLET_MNEMONIC?.split(' ') || [];
```

Create a `.env` file (and add it to `.gitignore`):

```
WALLET_MNEMONIC=word1 word2 word3 word4 word5 word6 word7 word8 word9 word10 word11 word12
```

### Step 2: Ensure You Have Testnet ADA

- Your wallet needs tADA (testnet ADA) to pay for transaction fees
- Each NFT minting costs approximately 0.2 tADA in fees
- Get testnet ADA from a Cardano testnet faucet

### Step 3: Configure Server Port (Optional)

The default port is 3000. You can change it:

```javascript
const PORT = 3000;  // Change to your preferred port
```

## How It Works

### Overview

1. Your microcontroller sends sensor data to the server
2. The server receives the data (temperature, humidity, timestamp)
3. It creates a unique name for this NFT (based on the timestamp)
4. It packages everything together: the sensor data, a name, and a description
5. It creates a blockchain transaction to mint (create) the NFT
6. It signs the transaction with your wallet (like signing a check)
7. It sends the transaction to the Cardano network
8. It gives you back a link to view your new NFT

### Detailed Process Flow

#### 1. Server Initialization

- Creates Express application instance
- Initializes Koios provider for Preprod testnet
- Creates MeshWallet instance from mnemonic
- Sets up CORS and JSON parsing middleware
- Starts listening on port 3000

#### 2. Receiving Sensor Data (POST /data)

When your microcontroller sends sensor data:

1. **Extract Data**: Server extracts `temperature`, `humidity`, and `timestamp` from request body
2. **Validate**: Checks that required fields (temperature, humidity) are present
3. **Get Wallet Info**: Retrieves UTXOs (available funds) and change address
4. **Create Policy**: Generates a forging script and Policy ID for the NFT collection
5. **Create Token Name**: Generates unique token name based on timestamp
6. **Build Metadata**: Creates NFT metadata following CIP-25 standard (label 721)
7. **Build Transaction**: Uses MeshTxBuilder to create the minting transaction
8. **Sign Transaction**: Signs with your wallet's private key
9. **Submit Transaction**: Sends transaction to Cardano network
10. **Return Response**: Returns transaction hash, explorer URL, and metadata

### Code Structure

#### Imports

```javascript
import express from 'express';
import cors from 'cors';
import { KoiosProvider, MeshWallet, MeshTxBuilder, ForgeScript, resolveScriptHash, stringToHex } from '@meshsdk/core';
```

- **express**: Web framework for creating the API server
- **cors**: Middleware for enabling CORS
- **@meshsdk/core**: Mesh SDK components for Cardano blockchain interactions

#### Provider and Wallet Setup

```javascript
const provider = new KoiosProvider('preprod');
const wallet = new MeshWallet({
    networkId: 0,  // 0 = testnet, 1 = mainnet
    fetcher: provider,
    submitter: provider,
    key: {
        type: 'mnemonic',
        words: mnemonic
    }
});
```

- **KoiosProvider**: Connects to Cardano Preprod testnet (free, no API key needed)
- **MeshWallet**: Wallet instance created from mnemonic phrase

#### POST /data Endpoint

**Request Body**:
```json
{
  "temperature": 23.5,
  "humidity": 65.2,
  "timestamp": 1705312200000
}
```

**Response** (Success):
```json
{
  "success": true,
  "message": "Sensor data received and NFT minted successfully",
  "txHash": "abc123...",
  "explorerUrl": "https://preprod.cardanoscan.io/transaction/abc123...",
  "policyId": "policy123...",
  "tokenName": "SensorData_1705312200000",
  "metadata": {
    "name": "Sensor Data NFT - 2024-01-15T10:30:00.000Z",
    "image": "https://cardanothings.io/nft.png",
    "mediaType": "image/png",
    "description": "Temperature and humidity sensor data",
    "author": "A CardanoThings.io User",
    "temperature": "23.5",
    "humidity": "65.2",
    "timestamp": 1705312200000
  }
}
```

#### NFT Metadata Structure

Each NFT contains:

- **Name**: A title for your NFT (like "Sensor Data NFT - 2024-01-15")
- **Description**: What the NFT is about
- **Image**: URL to the NFT image
- **Media Type**: Type of image file
- **Author**: Who created this NFT
- **Your Sensor Data**: The temperature, humidity, and timestamp

The metadata follows the **CIP-25 standard** (Cardano Improvement Proposal 25) for NFT metadata, using label 721.

## Usage

### Step 1: Start the Server

```bash
node server.js
```

You should see:
```
Server running on http://localhost:3000
POST sensor data to: http://localhost:3000/data
```

### Step 2: Send Sensor Data

Your microcontroller should send POST requests to `http://YOUR_COMPUTER_IP:3000/data` with JSON body:

```json
{
  "temperature": 23.5,
  "humidity": 65.2,
  "timestamp": 1705312200000
}
```

### Step 3: Monitor Server Logs

Watch the console for:
- "Received sensor data" messages
- "NFT minted successfully!" confirmations
- Transaction hashes
- Any error messages

### Step 4: View Your NFTs

1. Copy the `explorerUrl` from the API response
2. Open it in your browser to view the transaction on Cardano Explorer
3. View your NFTs in a Cardano wallet that supports NFTs

## Testing with Insomnia/Postman

### Test Health Check

1. Create a GET request to `http://localhost:3000/health`
2. You should receive: `{"status": "ok", "timestamp": "..."}`

### Test NFT Minting

1. Create a POST request to `http://localhost:3000/data`
2. Set Body type to **JSON**
3. Enter JSON payload:
   ```json
   {
     "temperature": 23.5,
     "humidity": 65.2,
     "timestamp": 1705312200000
   }
   ```
4. Click **Send**
5. You should receive a response with transaction hash and explorer URL

## API Endpoints

### POST /data

**Purpose**: Receive sensor data and mint NFT

**URL**: `http://localhost:3000/data`

**Method**: POST

**Request Body** (JSON):
```json
{
  "temperature": 23.5,    // Required: Temperature in Celsius
  "humidity": 65.2,       // Required: Humidity as percentage
  "timestamp": 1705312200000  // Optional: Timestamp in milliseconds
}
```

**Response** (Success - 200):
```json
{
  "success": true,
  "message": "Sensor data received and NFT minted successfully",
  "txHash": "abc123...",
  "explorerUrl": "https://preprod.cardanoscan.io/transaction/abc123...",
  "policyId": "policy123...",
  "tokenName": "SensorData_1705312200000",
  "metadata": { ... }
}
```

**Response** (Error - 400):
```json
{
  "success": false,
  "error": "temperature and humidity are required"
}
```

**Response** (Error - 500):
```json
{
  "success": false,
  "error": "Error message"
}
```

### GET /health

**Purpose**: Health check

**URL**: `http://localhost:3000/health`

**Method**: GET

**Response**:
```json
{
  "status": "ok",
  "timestamp": "2024-01-15T10:30:00.000Z"
}
```

## Important Security Tips

### Wallet Security

- **Never put your wallet passphrase directly in your code** - always use a `.env` file
- **Never commit `.env` files to version control** - add `.env` to `.gitignore`
- **Never share your wallet passphrase with anyone**
- **Use testnet for development** - We're using Preprod testnet which uses fake money - safe for testing!
- **Use mainnet only for production** - Change `networkId` to `1` and provider to `'api'` for mainnet

### Transaction Fees

- Make sure your wallet has some tADA for fees (about 0.2 ADA per NFT)
- Each NFT minting requires a transaction fee
- Fees are automatically calculated and deducted from your wallet

### Network Configuration

- **Testnet (Preprod)**: `networkId: 0`, `KoiosProvider('preprod')` - Safe for testing
- **Mainnet**: `networkId: 1`, `KoiosProvider('api')` - Real money, use with caution

## Troubleshooting

### Wallet Not Found / Invalid Mnemonic

**Error**: Wallet initialization fails

**Solutions**:
- Verify mnemonic phrase is correct (12 or 24 words)
- Ensure mnemonic words are in correct order
- Check that mnemonic is for the correct network (testnet vs mainnet)

### Insufficient Funds

**Error**: Transaction fails due to insufficient funds

**Solutions**:
- Ensure wallet has enough tADA (testnet ADA) for fees
- Each NFT minting costs approximately 0.2 tADA
- Get testnet ADA from a Cardano testnet faucet

### Transaction Submission Failed

**Error**: `Error minting NFT: [error message]`

**Solutions**:
- Check wallet has sufficient balance
- Verify network connection
- Check Koios API status
- Review server logs for detailed error messages
- Ensure UTXOs are available in wallet

### CORS Errors

**Error**: CORS policy blocking requests from microcontroller

**Solutions**:
- Ensure `app.use(cors())` is called before routes
- Verify microcontroller and server are on same network
- Check firewall settings

### Module Not Found

**Error**: `Cannot find module '@meshsdk/core'`

**Solutions**:
```bash
npm install @meshsdk/core express cors
```

### ES Module Syntax Error

**Error**: `SyntaxError: Cannot use import statement outside a module`

**Solutions**: Ensure `package.json` includes:
```json
{
  "type": "module"
}
```

## Customization

### Change NFT Image

Update the image URL in metadata:

```javascript
const assetMetadata = {
    name: `Sensor Data NFT - ${new Date().toISOString()}`,
    image: "https://your-domain.com/your-image.png",  // Change this
    mediaType: "image/png",
    // ...
};
```

### Add More Metadata Fields

Add custom fields to metadata:

```javascript
const assetMetadata = {
    // ... existing fields ...
    location: "San Francisco, CA",
    sensorId: "AHT10-001",
    customField: "your custom data"
};
```

### Change Policy Script

Use a different policy script:

```javascript
// Instead of withOneSignature, use other ForgeScript methods
const forgingScript = ForgeScript.withOneSignature(changeAddress);
// Or create custom policy scripts
```

### Change Network

Switch to mainnet (use with caution):

```javascript
const provider = new KoiosProvider('api');  // Mainnet
const wallet = new MeshWallet({
    networkId: 1,  // Mainnet
    // ...
});
```

## Next Steps

- **Add database storage**: Store sensor data and NFT metadata in a database
- **Add authentication**: Secure your API endpoints with API keys
- **Add rate limiting**: Prevent abuse of your API
- **Add error recovery**: Retry failed transactions automatically
- **Add logging**: Use a logging library like Winston or Pino
- **Add monitoring**: Monitor server health and transaction success rates
- **Add multiple sensors**: Support multiple sensor types and data formats
- **Add NFT burning**: Allow burning NFTs through API endpoint

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/03-input-and-write/mint-sensor-data-on-chain)

## Additional Resources

- [Express.js Documentation](https://expressjs.com/) - Official Express.js documentation
- [Mesh SDK Documentation](https://meshjs.dev/) - Mesh SDK for Cardano
- [CIP-25 Standard](https://cips.cardano.org/cips/cip25/) - NFT metadata standard
- [Koios API](https://www.koios.rest/) - Cardano blockchain data provider
- [Cardano Explorer](https://preprod.cardanoscan.io/) - View transactions on testnet
- [Cardano Improvement Proposals](https://cips.cardano.org/) - Cardano standards and proposals

## Notes

- Default port is 3000 - change if needed
- Uses Preprod testnet by default - safe for testing
- Each NFT minting costs approximately 0.2 tADA in fees
- NFT metadata follows CIP-25 standard (label 721)
- Policy ID is generated from wallet address - all NFTs from same wallet share same Policy ID
- Token names are unique based on timestamp
- Server automatically handles transaction building, signing, and submission

