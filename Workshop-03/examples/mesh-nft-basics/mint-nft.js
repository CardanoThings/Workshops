// Import Mesh SDK components needed for minting NFTs
// KoiosProvider: Connects to Cardano blockchain to read and submit data
// MeshWallet: Represents your wallet and handles signing transactions
// MeshTxBuilder: Builds blockchain transactions step by step
// ForgeScript: Creates the policy script that controls who can mint NFTs
// resolveScriptHash: Converts the policy script into a Policy ID
// stringToHex: Converts text names into hexadecimal format for blockchain
import { KoiosProvider, MeshWallet } from '@meshsdk/core';
import { MeshTxBuilder, ForgeScript, resolveScriptHash, stringToHex } from '@meshsdk/core';

// Step 1: Set up the blockchain provider
// This connects you to the Cardano network
// 'preprod' = testnet (free, for testing), 'api' = mainnet (real money)
const provider = new KoiosProvider('preprod');

// Step 2: Set up your wallet
// IMPORTANT: Replace these words with your actual wallet mnemonic phrase
// NEVER share your mnemonic with anyone or commit it to GitHub!
// In production, use environment variables: process.env.WALLET_MNEMONIC?.split(' ')
const mnemonic = ["word1", "word2", "word3", "word4", "word5", "word6", "word7", "word8", "word9", "word10", "word11", "word12"];

// Step 3: Create your wallet instance
// This wallet will be used to sign transactions and interact with the blockchain
const wallet = new MeshWallet({
	networkId: 0,        // 0 = testnet (Preprod), 1 = mainnet
	fetcher: provider,   // Provider for reading blockchain data (like your balance)
	submitter: provider, // Provider for sending transactions to the network
	key: {
		type: 'mnemonic',  // We're using a mnemonic phrase (12 or 24 words)
		words: mnemonic    // Your wallet's mnemonic words
	}
});

// Step 4: Initialize the wallet
// This loads your wallet's information from the blockchain
await wallet.init();

// Step 5: Get your wallet's UTXOs (Unspent Transaction Outputs)
// UTXOs are like coins in your wallet - they represent available funds
// We need these to pay for the transaction fees
const utxos = await wallet.getUtxos();

// Step 6: Get your change address
// This is your wallet address where any leftover funds will be sent back
const changeAddress = await wallet.getChangeAddress();

// Step 7: Create a forging script (minting policy)
// This script defines who can mint NFTs from this collection
// withOneSignature means only your wallet can mint NFTs with this policy
const forgingScript = ForgeScript.withOneSignature(changeAddress);

// Step 8: Prepare the NFT metadata
// This is the information that will be stored in your NFT
// It can include name, description, image, and any custom data
const demoAssetMetadata = {
	name: "Sensor Data NFT - 2024-01-15T10:30:00Z",  // Name of your NFT
	image: "https://cardanothings.io/nft.png",      // URL to the NFT image
	mediaType: "image/png",                          // Type of image file
	description: "Temperature and humidity sensor data",  // Description of the NFT
	author: "A CardanoThings.io User",              // Who created this NFT
	temperature: "23.5",                            // Sensor reading: temperature
	humidity: "65.2",                               // Sensor reading: humidity
	timestamp: Date.now(),                           // When this data was recorded
};

// Step 9: Generate the Policy ID
// The Policy ID is a unique identifier for your NFT collection
// All NFTs minted with the same policy belong to the same collection
const policyId = resolveScriptHash(forgingScript);
console.log("Policy ID:", policyId);

// Step 10: Create a unique token name
// Each NFT needs a unique name within the collection
// We add a timestamp to make sure each NFT has a different name
const tokenName = "TemperatureNFT" + Date.now().toString();

// Step 11: Convert token name to hexadecimal
// Blockchain requires names to be in hexadecimal format (base 16)
const tokenNameHex = stringToHex(tokenName);

// Step 12: Structure the metadata according to CIP-25 standard
// CIP-25 is the Cardano standard for NFT metadata
// The structure is: { policyId: { tokenName: { metadata } } }
const metadata = { 
	[policyId]: { 
		[tokenName]: { ...demoAssetMetadata } 
	} 
};

// Step 13: Create a transaction builder
// This tool helps us build the minting transaction step by step
const txBuilder = new MeshTxBuilder({
	fetcher: provider,   // Provider for fetching blockchain data
	verbose: false,     // Set to true for detailed logging (helpful for debugging)
});

// Step 14: Build the minting transaction
// This creates the transaction that will mint your NFT
const unsignedTx = await txBuilder
	.mint("1", policyId, tokenNameHex)      // Mint 1 NFT with the given policy and name
	.mintingScript(forgingScript)            // Use our policy script
	.metadataValue(721, metadata)           // Attach metadata (721 is the CIP-25 standard label)
	.changeAddress(changeAddress)           // Where to send any leftover funds
	.selectUtxosFrom(utxos)                 // Which UTXOs to use for payment
	.complete();                             // Finish building the transaction

// Step 15: Sign the transaction
// Your wallet signs the transaction to prove you authorized it
// This is like signing a check - it proves the transaction came from you
const signedTx = await wallet.signTx(unsignedTx);

// Step 16: Submit the transaction to the blockchain
// This sends your transaction to the Cardano network
// The network will process it and create your NFT
const txHash = await wallet.submitTx(signedTx);

// Step 17: Check if the transaction was successful
// If txHash exists, the transaction was submitted successfully
if (txHash) {
	console.log("Transaction submitted successfully!");
	console.log("Transaction Hash:", txHash);
	// You can view your transaction on the Cardano explorer
	console.log("View on Cardano Explorer:", `https://preprod.cardanoscan.io/transaction/${txHash}`);
} else {
	console.error("Transaction submission failed!");
}

