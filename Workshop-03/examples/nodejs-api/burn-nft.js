
import { KoiosProvider, MeshWallet } from '@meshsdk/core';
import { MeshTxBuilder, ForgeScript, resolveScriptHash, stringToHex } from '@meshsdk/core';

const mnemonic = process.env.WALLET_MNEMONIC?.split(' ') || ["word1", "word2", "word3", "word4", "word5", "word6", "word7", "word8", "word9", "word10", "word11", "word12"];

const tokenName = "" // the exact name of the token you want to burn;

// Initialize Koios provider for Preprod Testnet
const provider = new KoiosProvider(
    'preprod'
);

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

await wallet.init();

const utxos = await wallet.getUtxos();
const changeAddress = await wallet.getChangeAddress();
const forgingScript = ForgeScript.withOneSignature(changeAddress);

const policyId = resolveScriptHash(forgingScript);
const tokenNameHex = stringToHex(tokenName);

const txBuilder = new MeshTxBuilder({
    fetcher: provider,
    verbose: false, // set to true for detailed debugging information during transaction building
});

// build the transaction
const unsignedTx = await txBuilder
    .mint("-1", policyId, tokenNameHex)
    .mintingScript(forgingScript)
    .changeAddress(changeAddress)
    .selectUtxosFrom(utxos)
    .complete();

// sign the transaction
const signedTx = await wallet.signTx(unsignedTx);

// submit the transaction
const txHash = await wallet.submitTx(signedTx);

// log the transaction hash - you can view it on the cardano explorer at https://preprod.cardanoscan.io/transaction/{txHash}
if (txHash) {
    console.log("Transaction submitted successfully!");
    console.log("Transaction Hash:", txHash);
    console.log("View on Cardano Explorer:", `https://preprod.cardanoscan.io/transaction/${txHash}`);
} else {
    console.error("Transaction submission failed!");
}


