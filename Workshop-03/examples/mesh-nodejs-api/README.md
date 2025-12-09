# Complete Node.js API with Mesh Integration

This complete API server combines Express.js with Mesh to create a REST API that can interact with Cardano wallets and submit transactions. It includes endpoints to retrieve wallet information and receive sensor data that automatically creates and submits transactions to the blockchain. The wallet instance is initialized once at startup from your mnemonic, making it efficient for handling multiple requests.

## Overview

This complete API server combines Express.js with Mesh to create a REST API that can interact with Cardano wallets and submit transactions. It includes endpoints to retrieve wallet information and receive sensor data that automatically creates and submits transactions to the blockchain. The wallet instance is initialized once at startup from your mnemonic, making it efficient for handling multiple requests.

## Key Concepts

- **Express.js:** Web framework for creating the API server and handling HTTP requests.
- **KoiosProvider:** Data provider that connects to Koios API to fetch blockchain data. Koios is free to use and doesn't require an API key.
- **MeshWallet:** Wallet instance created from your mnemonic passphrase, initialized once at server startup.
- **MeshTxBuilder:** Low-level transaction builder that provides fine-grained control over transaction construction.
- **GET /wallet:** API endpoint that uses the wallet instance to fetch and return wallet information.
- **POST /data:** API endpoint that receives sensor data, automatically creates a transaction with the data as metadata, and submits it to the blockchain using the same transaction building code from earlier examples.

## How the API Works

1. **Server Initialization:** The server starts and initializes the Koios provider and Mesh wallet from the mnemonic array in the code.
2. **Wallet Setup:** The wallet is created once at startup, not per request, making it efficient.
3. **GET /wallet Endpoint:** When a GET request is made to `/wallet`:
   - The endpoint uses the pre-initialized wallet instance
   - Fetches the wallet address using `getChangeAddress()`
   - Fetches the wallet balance using `getBalance()` which returns an array of assets
   - Extracts the lovelace amount from the balance array
   - Converts from Lovelace to ADA
   - Returns JSON response with wallet information
4. **POST /data Endpoint:** When a POST request is made to `/data` with sensor data:
   - Validates the request body (temperature and humidity are required)
   - Extracts sensor data (temperature, humidity) from the request
   - Generates a timestamp server-side when the data is received
   - Sets the recipient address to the PingPong wallet address (same as in the transaction example earlier) - this wallet automatically refunds transactions minus fees within 60 seconds
   - Sets the transaction amount to 10 ADA
   - Creates transaction metadata with the sensor data formatted as messages (label 674, CIP-20 standard)
   - Uses the same transaction building code from earlier examples:
     - Gets wallet UTXOs and change address
     - Initializes MeshTxBuilder with verbose set to false
     - Builds the transaction with output, change address, and metadata
     - Selects UTXOs to fund the transaction
     - Signs the transaction with the wallet
     - Submits the transaction to the Cardano network
   - Returns the transaction hash, explorer URL, and the sensor data that was stored

## API Endpoints

- **GET /wallet:** Retrieves wallet information including address and balance. Uses the wallet instance initialized from the mnemonic array in the code.
- **POST /data:** Receives sensor data (temperature, humidity) and automatically generates a timestamp server-side. Creates and submits a transaction with the data and timestamp as metadata. Uses the same transaction building code from the transaction example earlier. The transaction is sent to the PingPong wallet address. Returns transaction hash, explorer URL, and sensor data including the server-generated timestamp.
- **GET /health:** Health check endpoint to verify the server is running.

## POST /data Request Body

The POST /data endpoint expects a JSON body with the following structure:

```json
{
  "temperature": 23.5,  // Required: Temperature in Celsius
  "humidity": 65.2      // Required: Humidity in %RH
}
```

**Note:** The timestamp is automatically generated server-side when the data is received. You don't need to include it in the request.

## Testing with Insomnia

1. Start your server with `node server.js`
2. Make sure you've replaced the example mnemonic words in the code with your actual testnet wallet mnemonic
3. **Test GET /wallet:**
   - Open Insomnia and create a new GET request to `http://localhost:3000/wallet`
   - Click Send to get your wallet information
   - You should receive a response with your wallet address and balance in Lovelace and ADA

## Security Considerations

- **Never expose private keys or mnemonics:** Keep sensitive wallet information secure and never include it in your API code or environment variables that might be exposed.
- **Testnet only:** Always use 'preprod' for testnet development. Only switch to 'api' (mainnet) when deploying production applications.
- **Input validation:** The endpoint validates required fields and amount values. In production, add more robust validation and sanitization.
- **Rate limiting:** Implement rate limiting to prevent abuse of your API endpoints. Koios has rate limits, so be mindful of request frequency.
- **Transaction fees:** Make sure your wallet has enough balance to cover transaction amounts plus fees (usually around 0.17-0.2 ADA).
- **No API key needed:** Koios is free and doesn't require an API key, making it perfect for development and testing.

