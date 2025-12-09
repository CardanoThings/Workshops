// Import required Node.js packages
import express from 'express';
import cors from 'cors';
import { KoiosProvider, MeshWallet, MeshTxBuilder } from '@meshsdk/core';

// Create Express application instance
const app = express();

// Server port number
const PORT = 3000;

// Initialize Koios provider for Preprod Testnet
// Koios is free to use and doesn't require an API key
const provider = new KoiosProvider(
	'preprod'  // Network: 'preprod' for testnet, 'api' for mainnet
);

// Initialize wallet using mnemonic
// WARNING: This is for example purposes only! Never hardcode your mnemonic in production code!
// In production, always use environment variables: process.env.WALLET_MNEMONIC?.split(' ') || []
// Replace with your actual 12 or 24 word mnemonic phrase from your testnet wallet
const mnemonic = ["word1", "word2", "word3", "word4", "word5", "word6", "word7", "word8", "word9", "word10", "word11", "word12"];

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

// Middleware: Enable CORS to allow requests from different origins
// This allows your microcontroller to make requests to this API from a different domain
app.use(cors());

// Middleware: Parse JSON request bodies
// This automatically parses JSON data sent in POST/PUT requests
app.use(express.json());

// GET endpoint to retrieve wallet information
// Returns wallet address, balance, and network information
app.get('/wallet', async (req, res) => {
	try {
		// Get wallet address
		// The change address is the address where change from transactions is sent
		const address = await wallet.getChangeAddress();
		
		// Get wallet balance using Mesh's built-in method
		// Returns an array of assets: [{ unit: 'lovelace', quantity: '...' }, ...]
		// The first item is always lovelace (ADA)
		const balanceArray = await wallet.getBalance();
		
		// Extract lovelace from the balance array
		// Find the item with unit 'lovelace' and get its quantity
		const lovelaceAsset = balanceArray.find(asset => asset.unit === 'lovelace');
		const balanceLovelace = lovelaceAsset ? parseInt(lovelaceAsset.quantity) : 0;
		
		// Convert Lovelace to ADA
		// 1 ADA = 1,000,000 Lovelace
		const balanceADA = balanceLovelace / 1000000;
		
		// Return wallet information as JSON response
		res.json({ 
			success: true, 
			address: address,  // Wallet address
			balance: {
				lovelace: balanceLovelace,  // Balance in Lovelace
				ada: balanceADA  // Balance in ADA
			},
			network: 'preprod'  // Network: preprod testnet
		});
	} catch (error) {
		// Handle errors and return error response
		console.error('Error:', error);
		res.status(500).json({ success: false, error: error.message });
	}
});

// POST endpoint to receive sensor data and create a transaction
// URL: http://localhost:3000/data
// Request Body: { temperature: 23.5, humidity: 65.2 }
app.post('/data', async (req, res) => {
	try {
		// Extract sensor data from request body
		const { temperature, humidity } = req.body;
		
		// Validate required fields
		if (temperature === undefined || humidity === undefined) {
			return res.status(400).json({ 
				success: false, 
				error: 'temperature and humidity are required' 
			});
		}

		// Generate timestamp server-side when data is received
		const timestamp = Date.now();

		console.log('Received sensor data:', { temperature, humidity, timestamp });

		// PingPong wallet address - this wallet will automatically refund the transaction minus fees within 60 seconds
		// Perfect for testing transactions on the Cardano Preprod testnet
		const recipientAddress = 'addr_test1qpvla0l6zgkl4ufzur0wal0uny5lyqsg4rw7g6gxj08lzacth0hnd66lz6uqqz7kwkmx07xyppsk2cddvxnqvfd05reqf7p26w';

		// Amount to send in ADA (convert to Lovelace: 1 ADA = 1,000,000 Lovelace)
		const amountADA = 10.0;  // Send 10 ADA
		const amountLovelace = Math.floor(amountADA * 1000000);

		// Create transaction metadata with sensor data
		// Label 674 = Message (CIP-20 standard for transaction messages)
		const transactionMetadata = {
			674: {  // Message label (CIP-20 standard)
				msg: [
					`Sensor Data: Temperature ${temperature}°C, Humidity ${humidity}%RH`,
					`Timestamp: ${timestamp}`
				]
			}
		};

		// Get wallet UTXOs (Unspent Transaction Outputs)
		// UTXOs represent available funds in your wallet that can be spent
		const utxos = await wallet.getUtxos();
		
		// Get change address
		// This is where any remaining funds (after transaction amount and fees) will be sent
		const changeAddress = await wallet.getChangeAddress();
		
		// Initialize MeshTxBuilder
		// MeshTxBuilder provides low-level APIs for building transactions
		const txBuilder = new MeshTxBuilder({
			fetcher: provider,  // Provider for fetching blockchain data
			verbose: false  // Set to true for detailed debugging information during transaction building
		});
		
		// Build the transaction using MeshTxBuilder
		// This uses the same code pattern as the POST /transaction endpoint
		const unsignedTx = await txBuilder
			.txOut(recipientAddress, [{ unit: 'lovelace', quantity: amountLovelace.toString() }])  // Output: send lovelace to recipient
			.changeAddress(changeAddress)  // Address to receive change
			.metadataValue(674, transactionMetadata[674])  // Attach metadata with sensor data (label 674)
			.selectUtxosFrom(utxos)  // Automatically select UTXOs to fund the transaction
			.complete();  // Finalize the transaction structure
		
		// Sign the transaction with your wallet's private key
		// This proves that you own the wallet and authorizes the transaction
		const signedTx = await wallet.signTx(unsignedTx);
		
		// Submit the signed transaction to the Cardano network
		// The transaction will be broadcast to the network and included in the next block
		const txHash = await wallet.submitTx(signedTx);
		
		// Return success response with transaction details
		res.json({ 
			success: true, 
			message: 'Sensor data received and transaction submitted successfully',
			txHash: txHash,  // Transaction hash (unique identifier)
			explorerUrl: `https://preprod.cardanoscan.io/transaction/${txHash}`,  // Link to view transaction on explorer
			sensorData: {
				temperature: temperature,
				humidity: humidity,
				timestamp: timestamp  // Timestamp generated server-side when data was received
			}
		});
	} catch (error) {
		// Handle errors and return error response
		console.error('Error processing sensor data or submitting transaction:', error);
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
});

