// Import required Node.js packages
import express from 'express';
import cors from 'cors';
import { KoiosProvider, MeshWallet, MeshTxBuilder, ForgeScript, resolveScriptHash, stringToHex } from '@meshsdk/core';

// Create Express application instance
const app = express();

// Server port number
const PORT = 3000;

// Initialize Koios provider for Preprod Testnet
// Koios is free to use and doesn't require an API key
const provider = new KoiosProvider('preprod');

// Initialize wallet using mnemonic from environment variable
// IMPORTANT: Store your mnemonic in an environment variable, never in code!
const mnemonic = process.env.WALLET_MNEMONIC?.split(' ') || [];

if (mnemonic.length === 0) {
    console.warn('Warning: WALLET_MNEMONIC not set. Minting functionality will not work.');
}

const wallet = new MeshWallet({
    networkId: 0,  // 0 = testnet
    fetcher: provider,
    submitter: provider,
    key: {
        type: 'mnemonic',
        words: mnemonic
    }
});

// Middleware: Enable CORS to allow requests from different origins
app.use(cors());

// Middleware: Parse JSON request bodies
app.use(express.json());

// POST endpoint to receive sensor data and mint NFT
// URL: http://localhost:3000/sensor-data
app.post('/sensor-data', async (req, res) => {
    try {
        if (mnemonic.length === 0) {
            return res.status(400).json({ 
                success: false, 
                error: 'Wallet mnemonic not configured. Set WALLET_MNEMONIC environment variable.' 
            });
        }

        // Extract sensor data from request body
        const { temperature, humidity, timestamp, sensor_type } = req.body;

        // Validate required fields
        if (temperature === undefined || humidity === undefined) {
            return res.status(400).json({ 
                success: false, 
                error: 'temperature and humidity are required' 
            });
        }

        console.log('Received sensor data:', { temperature, humidity, timestamp, sensor_type });

        // Get wallet UTXOs and change address
        const utxos = await wallet.getUtxos();
        const changeAddress = await wallet.getChangeAddress();

        // Create forging script for minting
        // This creates a simple policy that allows minting from the wallet address
        const forgingScript = ForgeScript.withOneSignature(changeAddress);
        const policyId = resolveScriptHash(forgingScript);

        // Create unique token name based on timestamp
        const tokenName = \