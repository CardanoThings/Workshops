
import { KoiosProvider, MeshWallet } from '@meshsdk/core';
import { MeshTxBuilder, ForgeScript, resolveScriptHash, stringToHex } from '@meshsdk/core';

const provider = new KoiosProvider(
    'preprod'  // Network: 'preprod' for testnet, 'api' for mainnet
);

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

await wallet.init();

const utxos = await wallet.getUtxos();
const changeAddress = await wallet.getChangeAddress();
const forgingScript = ForgeScript.withOneSignature(changeAddress);

const demoAssetMetadata = {
    name: "Sensor Data NFT - 2024-01-15T10:30:00Z",
    image: "https://cardanothings.io/nft.png",
    mediaType: "image/png",
    description: "Temperature and humidity sensor data",
    author: "A CardanoThings.io User",
    temperature: "23.5",
    humidity: "65.2",
    timestamp: Date.now(),
};

const policyId = resolveScriptHash(forgingScript);
console.log("Policy ID:", policyId);
const tokenName = "TemperatureNFT" + Date.now().toString();
const tokenNameHex = stringToHex(tokenName);
const metadata = { [policyId]: { [tokenName]: { ...demoAssetMetadata } } };

const txBuilder = new MeshTxBuilder({
    fetcher: provider,
    verbose: false,
});

// build the transaction
const unsignedTx = await txBuilder
    .mint("1", policyId, tokenNameHex)
    .mintingScript(forgingScript)
    .metadataValue(721, metadata)
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
