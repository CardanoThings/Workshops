// Import Mesh SDK components
import { KoiosProvider, MeshWallet } from '@meshsdk/core';

// Initialize Koios provider for Preprod Testnet
// Koios is free to use and doesn't require an API key
// 'preprod' = Preprod testnet, 'api' = Mainnet
const provider = new KoiosProvider('preprod');

// Initialize wallet using mnemonic
// WARNING: This is for example purposes only! Never hardcode your mnemonic in production code!
// In production, always use environment variables: process.env.WALLET_MNEMONIC?.split(' ') || []
// Replace with your actual 12 or 24 word mnemonic phrase from your testnet wallet
const mnemonic = process.env.WALLET_MNEMONIC?.split(' ') || ["word1", "word2", "word3", "word4", "word5", "word6", "word7", "word8", "word9", "word10", "word11", "word12"];

// Create MeshWallet instance
// This wallet will be used to interact with the Cardano blockchain
const wallet = new MeshWallet({
    networkId: 0,  // 0 = testnet (Preprod), 1 = mainnet
    fetcher: provider,  // Provider for fetching blockchain data
    submitter: provider,  // Provider for submitting transactions
    key: {
        type: 'mnemonic',  // Wallet key type: mnemonic phrase
        words: mnemonic  // Array of mnemonic words
    }
});

// Function to fetch and log wallet balance
async function fetchWalletBalance() {
    try {
        // Get wallet address
        // The change address is the address where change from transactions is sent
        const address = await wallet.getChangeAddress();
        console.log('Wallet Address:', address);

        // Get wallet balance using Mesh's built-in method
        // Returns an array of assets: [{ unit: 'lovelace', quantity: '...' }, ...]
        // The first item is always lovelace (ADA), followed by any native tokens
        const balanceArray = await wallet.getBalance();

        // Extract lovelace from the balance array
        // Find the item with unit 'lovelace' and get its quantity
        const lovelaceAsset = balanceArray.find(asset => asset.unit === 'lovelace');
        const balanceLovelace = lovelaceAsset ? parseInt(lovelaceAsset.quantity) : 0;

        // Convert Lovelace to ADA
        // 1 ADA = 1,000,000 Lovelace
        const balanceADA = balanceLovelace / 1000000;

        // Log wallet information to console
        console.log('Wallet Balance:', balanceADA, 'ADA');
        console.log('Balance in Lovelace:', balanceLovelace);
    } catch (error) {
        // Handle any errors that occur during balance fetching
        console.error('Error fetching wallet balance:', error);
    }
}

// Call the function to fetch and log wallet balance
fetchWalletBalance();