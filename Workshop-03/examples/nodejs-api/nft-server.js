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

// Initialize wallet using mnemonic
// IMPORTANT: Replace these words with your actual wallet mnemonic phrase
// NEVER share your mnemonic with anyone or commit it to GitHub!
const mnemonic = ["word1", "word2", "word3", "word4", "word5", "word6", "word7", "word8", "word9", "word10", "word11", "word12"];

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
// URL: http://localhost:3000/data
app.post('/data', async (req, res) => {
    try {
        // Extract sensor data from request body
        const { temperature, humidity, timestamp } = req.body;

        // Validate required fields
        if (temperature === undefined || humidity === undefined) {
            return res.status(400).json({
                success: false,
                error: 'temperature and humidity are required'
            });
        }

        console.log('Received sensor data:', { temperature, humidity, timestamp });

        // Get wallet UTXOs and change address
        const utxos = await wallet.getUtxos();
        const changeAddress = await wallet.getChangeAddress();

        // Create forging script for minting
        // This creates a simple policy that allows minting from the wallet address
        const forgingScript = ForgeScript.withOneSignature(changeAddress);
        const policyId = resolveScriptHash(forgingScript);

        // Create unique token name based on timestamp
        const tokenName = `SensorData_${timestamp || Date.now()}`;
        const tokenNameHex = stringToHex(tokenName);

        // Create NFT metadata following CIP-25 standard (label 721)
        const assetMetadata = {
            name: `Sensor Data NFT - ${new Date().toISOString()}`,
            image: "https://cardanothings.io/nft.png",
            mediaType: "image/png",
            description: 'Temperature and humidity sensor data',
            author: "A CardanoThings.io User",
            temperature: temperature.toString(),
            humidity: humidity.toString(),
            timestamp: timestamp || Date.now()
        };

        // Structure metadata according to CIP-25 standard
        const metadata = {
            [policyId]: {
                [tokenName]: assetMetadata
            }
        };

        // Initialize MeshTxBuilder
        const txBuilder = new MeshTxBuilder({
            fetcher: provider,
            verbose: true
        });

        // Build the minting transaction
        const unsignedTx = await txBuilder
            .mint("1", policyId, tokenNameHex)  // Mint 1 token
            .mintingScript(forgingScript)         // Use the forging script
            .metadataValue(721, metadata)         // Attach NFT metadata (CIP-25 standard)
            .changeAddress(changeAddress)        // Address to receive change
            .selectUtxosFrom(utxos)              // Select UTXOs to fund the transaction
            .complete();

        // Sign the transaction with your wallet
        const signedTx = await wallet.signTx(unsignedTx);

        // Submit the transaction to the network
        const txHash = await wallet.submitTx(signedTx);

        console.log('NFT minted successfully!');
        console.log('Transaction Hash:', txHash);

        res.json({
            success: true,
            message: 'Sensor data received and NFT minted successfully',
            txHash: txHash,
            explorerUrl: `https://preprod.cardanoscan.io/transaction/${txHash}`,
            policyId: policyId,
            tokenName: tokenName,
            metadata: assetMetadata
        });
    } catch (error) {
        console.error('Error minting NFT:', error);
        res.status(500).json({ success: false, error: error.message });
    }
});

// GET endpoint for health check
app.get('/health', (req, res) => {
    res.json({ status: 'ok', timestamp: new Date().toISOString() });
});

// Start server and listen on specified port
app.listen(PORT, () => {
    console.log(`Server running on http://localhost:${PORT}`);
    console.log('POST sensor data to: http://localhost:' + PORT + '/data');
});