// Import Mesh SDK components needed for burning NFTs
import { KoiosProvider, MeshWallet } from '@meshsdk/core';
import { MeshTxBuilder, ForgeScript, resolveScriptHash, stringToHex } from '@meshsdk/core';

// IMPORTANT: Replace these words with your actual wallet mnemonic phrase
// NEVER share your mnemonic with anyone or commit it to GitHub!
const mnemonic = ["word1", "word2", "word3", "word4", "word5", "word6", "word7", "word8", "word9", "word10", "word11", "word12"];

// The exact name of the token you want to burn
// This should match the tokenName used when minting the NFT
const tokenName = ""; // Replace with your token name, e.g., "SensorData_1705312200000"

// Initialize Koios provider for Preprod Testnet
const provider = new KoiosProvider('preprod');

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

// Initialize the wallet to load its information from the blockchain
await wallet.init();

// Get wallet UTXOs (Unspent Transaction Outputs) - these are like coins in your wallet
const utxos = await wallet.getUtxos();

// Get the change address where any leftover funds will be sent back
const changeAddress = await wallet.getChangeAddress();

// Create forging script for the policy
// This must match the policy used when minting the NFT
const forgingScript = ForgeScript.withOneSignature(changeAddress);

// Generate the Policy ID from the forging script
const policyId = resolveScriptHash(forgingScript);

// Convert token name to hexadecimal format (required by blockchain)
const tokenNameHex = stringToHex(tokenName);

// Initialize transaction builder
const txBuilder = new MeshTxBuilder({
	fetcher: provider, // Provider for fetching blockchain data
	verbose: false, // Set to true for detailed debugging information during transaction building
});

// Build the burn transaction
// Minting "-1" is the same as burning 1 token
const unsignedTx = await txBuilder
	.mint("-1", policyId, tokenNameHex)  // Mint -1 token (burns 1 token)
	.mintingScript(forgingScript)         // Use the same policy script
	.changeAddress(changeAddress)        // Address to receive change
	.selectUtxosFrom(utxos)               // Select UTXOs to fund the transaction
	.complete();

// Sign the transaction with your wallet
const signedTx = await wallet.signTx(unsignedTx);

// Submit the transaction to the blockchain
const txHash = await wallet.submitTx(signedTx);

// Log the transaction hash - you can view it on the Cardano explorer
if (txHash) {
	console.log("Transaction submitted successfully!");
	console.log("Transaction Hash:", txHash);
	console.log("View on Cardano Explorer:", `https://preprod.cardanoscan.io/transaction/${txHash}`);
} else {
	console.error("Transaction submission failed!");
}

