// Import Mesh SDK components
import { KoiosProvider, MeshWallet, MeshTxBuilder } from '@meshsdk/core';

// Initialize Koios provider for Preprod Testnet
// Koios is free to use and doesn't require an API key
// 'preprod' = Preprod testnet, 'api' = Mainnet
const provider = new KoiosProvider('preprod');

// Initialize wallet using mnemonic
// WARNING: This is for example purposes only! Never hardcode your mnemonic in production code!
// In production, always use environment variables: process.env.WALLET_MNEMONIC?.split(' ') || []
// Replace with your actual 12 or 24 word mnemonic phrase from your testnet wallet
const mnemonic = ["word1", "word2", "word3", "word4", "word5", "word6", "word7", "word8", "word9", "word10", "word11", "word12"];

// Create MeshWallet instance
// This wallet will be used to create and sign transactions
const wallet = new MeshWallet({
	networkId: 0,  // 0 = testnet (Preprod), 1 = mainnet
	fetcher: provider,  // Provider for fetching blockchain data
	submitter: provider,  // Provider for submitting transactions
	key: {
		type: 'mnemonic',  // Wallet key type: mnemonic phrase
		words: mnemonic  // Array of mnemonic words
	}
});

// Function to create and submit a transaction with metadata
async function sendTransaction() {
	try {
		// PingPong wallet address - this wallet will automatically refund the transaction minus fees within 60 seconds
		// Perfect for testing transactions on the Cardano Preprod testnet
		// The PingPong wallet sends your funds back automatically, making it ideal for testing
		const recipientAddress = 'addr_test1qpvla0l6zgkl4ufzur0wal0uny5lyqsg4rw7g6gxj08lzacth0hnd66lz6uqqz7kwkmx07xyppsk2cddvxnqvfd05reqf7p26w';
		
		// Amount to send in ADA
		// This will be converted to Lovelace (1 ADA = 1,000,000 Lovelace)
		const amountADA = 10.0;  // Send 10 ADA
		const amountLovelace = Math.floor(amountADA * 1000000);  // Convert to Lovelace
		
		// Transaction metadata
		// Metadata allows you to attach additional data to transactions that is permanently stored on the blockchain
		// Metadata labels must be numbers between 0 and 65535
		// Label 674 = Message (CIP-20 standard for transaction messages)
		const metadata = {
			674: {  // Message label (CIP-20 standard)
				msg: ['Hello from CardanoThings!', 'This is a test transaction with metadata.']
			}
		};
		
		// Log transaction details before creating it
		console.log('Creating transaction...');
		console.log('Recipient:', recipientAddress);
		console.log('Amount:', amountADA, 'ADA');
		console.log('Metadata:', JSON.stringify(metadata, null, 2));
		
		// Get wallet UTXOs (Unspent Transaction Outputs)
		// UTXOs represent available funds in your wallet that can be spent
		const utxos = await wallet.getUtxos();
		
		// Get change address
		// This is where any remaining funds (after transaction amount and fees) will be sent
		const changeAddress = await wallet.getChangeAddress();
		
		// Initialize MeshTxBuilder
		// MeshTxBuilder provides low-level APIs for building transactions with fine-grained control
		// This gives you more control than the higher-level wallet.buildTx() method
		const txBuilder = new MeshTxBuilder({
			fetcher: provider,  // Provider for fetching blockchain data needed for transaction building
			verbose: false  // Set to true for detailed debugging information during transaction building
		});
		
		// Build the transaction using MeshTxBuilder
		// This approach gives you more control over the transaction structure
		const unsignedTx = await txBuilder
			.txOut(recipientAddress, [{ unit: 'lovelace', quantity: amountLovelace.toString() }])  // Output: send lovelace to recipient address
			.changeAddress(changeAddress)  // Address to receive change (remaining funds after transaction)
			.metadataValue(674, metadata[674])  // Attach message metadata (label 674, CIP-20 standard)
			.selectUtxosFrom(utxos)  // Automatically select UTXOs from the provided list to fund the transaction
			.complete();  // Finalize the transaction structure and return the unsigned transaction
		
		// Sign the transaction with your wallet's private key
		// This proves that you own the wallet and authorizes the transaction
		const signedTx = await wallet.signTx(unsignedTx);
		
		// Submit the signed transaction to the Cardano network
		// The transaction will be broadcast to the network and included in the next block
		const txHash = await wallet.submitTx(signedTx);
		
		// Log success message and transaction details
		console.log('Transaction submitted successfully!');
		console.log('Transaction Hash:', txHash);
		console.log('View on Cardano Explorer:', `https://preprod.cardanoscan.io/transaction/${txHash}`);
		console.log('Metadata will be visible on the blockchain explorer');
	} catch (error) {
		// Handle any errors that occur during transaction creation or submission
		console.error('Error creating or submitting transaction:', error);
	}
}

// Call the function to create and submit transaction
sendTransaction();

