# Mesh Basics Examples

This directory contains two fundamental Mesh.js examples that demonstrate how to interact with the Cardano blockchain:

1. **wallet-balance.js** - Fetch and display wallet balance
2. **send-transaction.js** - Create and submit transactions with metadata

Both examples use the Mesh SDK with the Koios provider to interact with the Cardano Preprod testnet.

## Overview

These examples demonstrate the core concepts of working with Mesh.js:
- Initializing a wallet from a mnemonic phrase
- Fetching wallet information and balances
- Building and submitting transactions with metadata

## Requirements

- **Node.js** (v14 or higher recommended)
- **npm** (Node Package Manager)
- **@meshsdk/core** - Mesh SDK for Cardano
- A Cardano testnet wallet with mnemonic phrase
- A code editor (VS Code, Cursor, etc.)

## Installation

### Step 1: Install Node.js

If you don't have Node.js installed:

1. Visit [nodejs.org](https://nodejs.org/)
2. Download and install the LTS (Long Term Support) version
3. Verify installation by running: `node --version` and `npm --version`

### Step 2: Create Project Directory

```bash
mkdir mesh-basics
cd mesh-basics
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

## Example 1: Fetch Wallet Balance

### Overview

This simple example demonstrates how to use Mesh with the Koios provider to fetch and display your wallet balance. It's a standalone script that initializes a wallet from your mnemonic and logs the balance information to the console.

### Key Concepts

- **KoiosProvider:** A data provider that connects to Koios API to fetch blockchain data. Koios is free to use and doesn't require an API key.
- **MeshWallet:** A wallet instance created from your mnemonic passphrase. It provides methods to interact with your wallet.
- **getChangeAddress():** Returns the wallet's change address (the address where change from transactions is sent).
- **getBalance():** Fetches the wallet balance as an array of assets. Returns `[{ unit: 'lovelace', quantity: '...' }, ...]` where the first item is always lovelace (ADA). This is a convenient method that automatically calculates the balance from all available funds.

### How the Code Works

1. **Initialize Provider:** Creates a KoiosProvider instance for the Preprod testnet.
2. **Load Mnemonic:** Reads the mnemonic from the mnemonic array in the code.
3. **Create Wallet:** Initializes a MeshWallet instance with the mnemonic and provider.
4. **Fetch Balance:** The `fetchWalletBalance()` function:
   - Gets the wallet address using `getChangeAddress()`
   - Fetches the wallet balance using `getBalance()` which returns an array of assets
   - Extracts the lovelace amount from the array by finding the asset with `unit === 'lovelace'`
   - Converts from Lovelace to ADA (1 ADA = 1,000,000 Lovelace)
   - Logs the wallet address and balance to the console

### Running the Script

1. Make sure you have `@meshsdk/core` installed: `npm install @meshsdk/core`
2. Update the mnemonic array in `wallet-balance.js` with your testnet wallet mnemonic
3. Run the script: `node wallet-balance.js`
4. You should see your wallet address and balance in ADA and Lovelace printed to the console

## Example 2: Create and Submit Transaction

### Overview

This example demonstrates how to create and submit a Cardano transaction using Mesh. It shows how to send ADA from your wallet to another address on the testnet, including metadata. The transaction is built, signed, and submitted to the blockchain.

### Key Concepts

- **MeshTxBuilder:** A low-level transaction builder that provides fine-grained control over transaction construction. It's the foundation that higher-level APIs like `wallet.buildTx()` use internally.
- **Transaction Building:** Creating a transaction structure that specifies inputs (UTXOs to spend), outputs (recipient address and amount), and metadata.
- **UTXO Selection:** The process of choosing which unspent transaction outputs (UTXOs) to use as inputs for funding the transaction. `selectUtxosFrom()` automatically selects the necessary UTXOs.
- **Change Address:** The address that receives any remaining funds after the transaction (input amount minus output amount minus fees).
- **Transaction Metadata:** Additional data attached to transactions that can include messages, timestamps, or any structured information. Metadata is permanently stored on the blockchain.
- **Metadata Labels:** Numbers (0-65535) that categorize metadata. Common labels include 674 (message, CIP-20) and 721 (NFT metadata, CIP-25).
- **Transaction Signing:** Cryptographically signing the transaction with your wallet's private key to prove ownership.
- **Transaction Submission:** Sending the signed transaction to the Cardano network for processing.
- **Lovelace:** The smallest unit of ADA (1 ADA = 1,000,000 Lovelace). Transactions use Lovelace for precision.
- **Transaction Hash:** A unique identifier for the transaction that can be used to track it on the blockchain explorer.

### How the Code Works

1. **Initialize Wallet:** Creates a MeshWallet instance from your mnemonic, same as the balance example.
2. **Set Transaction Details:** Defines the recipient address and amount to send (in ADA, then converted to Lovelace).
3. **Create Metadata:** Defines metadata with label 674 (CIP-20 standard for messages) containing an array of message strings.
4. **Get Wallet Data:** Fetches UTXOs (unspent transaction outputs) and the change address from the wallet. UTXOs are needed to fund the transaction, and the change address receives any remaining funds after the transaction.
5. **Initialize MeshTxBuilder:** Creates a `MeshTxBuilder` instance with the provider. `MeshTxBuilder` provides low-level APIs for building transactions with more control than the higher-level `wallet.buildTx()` method. The `verbose` option is set to `false` by default, but you can set it to `true` for detailed debugging information during transaction building.
6. **Build Transaction:** Uses `MeshTxBuilder` to construct the transaction:
   - `.txOut()` - Specifies the output: recipient address and amount in lovelace
   - `.changeAddress()` - Sets the address to receive change (remaining funds after transaction)
   - `.metadataValue()` - Attaches metadata to the transaction (label 674 for messages)
   - `.selectUtxosFrom()` - Automatically selects UTXOs from the provided list to fund the transaction
   - `.complete()` - Finalizes the transaction structure and returns the unsigned transaction
7. **Sign Transaction:** Uses `wallet.signTx()` to cryptographically sign the transaction with your wallet's private key.
8. **Submit Transaction:** Uses `wallet.submitTx()` to send the signed transaction to the Cardano network via the Koios provider.
9. **Get Transaction Hash:** The submission returns a transaction hash that can be used to track the transaction on a blockchain explorer.

### Transaction Metadata

Metadata allows you to attach additional information to transactions that is permanently stored on the blockchain. This is useful for:

- **Messages:** Adding text messages or notes to transactions (label 674)
- **Tracking:** Including timestamps, source information, or version numbers
- **NFT Data:** Storing NFT metadata (label 721) for minting operations
- **Custom Data:** Any structured JSON data you want to store on-chain

**Metadata Structure:**
- Metadata labels must be numbers between 0 and 65535
- Each label can contain any valid JSON structure
- Multiple metadata labels can be attached to a single transaction
- Metadata increases transaction size and fees slightly

**Common Metadata Labels:**
- **674:** Message metadata (CIP-20 standard)
- **721:** NFT metadata (CIP-25 standard)
- **100-999:** Custom labels for application-specific data

### Transaction Process

The transaction process follows these steps:

1. **Build:** Create the transaction structure with inputs (UTXOs to spend), outputs (recipient address and amount), and metadata.
2. **Calculate Fees:** The network automatically calculates transaction fees based on transaction size (including metadata) and network conditions.
3. **Sign:** Sign the transaction with your private key to prove you own the funds being spent.
4. **Submit:** Send the signed transaction to the network for validation and inclusion in a block.
5. **Confirm:** Wait for the transaction to be included in a block (usually takes a few seconds to minutes).

### Running the Script

1. Make sure you have `@meshsdk/core` installed: `npm install @meshsdk/core`
2. Update the mnemonic array in `send-transaction.js` with your testnet wallet mnemonic
3. Update the `recipientAddress` variable with a valid testnet address (or use the PingPong wallet address for testing)
4. Customize the metadata object to include your own data (messages, timestamps, etc.)
5. Make sure your wallet has enough tADA to cover the transaction amount plus fees
6. Run the script: `node send-transaction.js`
7. You should see the transaction hash printed to the console
8. Use the Cardano Explorer link to view your transaction and its metadata on the blockchain

### Important Notes

- **Testnet Only:** Always use testnet addresses (starting with `addr_test1`) for development.
- **Transaction Fees:** Every transaction requires a small fee (usually around 0.17-0.2 ADA). Metadata increases the transaction size, which may slightly increase fees.
- **Irreversible:** Once submitted, transactions cannot be reversed. Always double-check the recipient address and amount.
- **Network Confirmation:** Transactions may take a few seconds to minutes to be confirmed and appear on the blockchain.
- **Change Output:** If you send less than your total balance, the remaining amount (minus fees) is automatically sent back to your wallet as change.
- **Metadata Visibility:** All metadata is publicly visible on the blockchain explorer. Don't include sensitive information.

### Viewing Metadata

After submitting a transaction with metadata, you can view it on Cardano blockchain explorers:

- Open the transaction hash link provided in the console output
- Look for the "Metadata" or "Transaction Metadata" section
- You'll see all metadata labels and their associated data
- Metadata is permanently stored and cannot be modified or deleted

### Testing Transactions

For testing, you can use the CardanoThings PingPong wallet address (available in the Cardano Setup workshop). The PingPong wallet automatically sends transactions back to you, making it perfect for testing transaction flows with metadata.

## Security Notes

- Always store your mnemonic in environment variables, never in code
- Use testnet wallets for development and testing
- Never commit files containing mnemonics to version control
- Always double-check recipient addresses before submitting transactions
- Start with small amounts when testing transactions

