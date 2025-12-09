# Node.js Express API Basics

Simple Express.js API server examples - from basic health check to enhanced API with POST endpoints

## Overview

This directory contains two Express.js API server examples:

1. **Basic API** (`basic-api.js`) - Minimal server with a health check endpoint
2. **Enhanced API** (`server.js`) - Full-featured API with CORS, POST/GET endpoints, and data storage

Both examples demonstrate how to build REST APIs using Express.js and are perfect starting points for creating APIs that can receive data from microcontrollers.

## Requirements

- **Node.js** (v14 or higher recommended)
- **npm** (Node Package Manager)
- **Express.js** - Web framework for building APIs
- **CORS** (for enhanced API) - Middleware for enabling Cross-Origin Resource Sharing
- A code editor (VS Code, Cursor, etc.)
- **Optional**: API testing tool like Insomnia or Postman

## Installation

### Step 1: Install Node.js

If you don't have Node.js installed:

1. Visit [nodejs.org](https://nodejs.org/)
2. Download and install the LTS (Long Term Support) version
3. Verify installation by running: `node --version` and `npm --version`

### Step 2: Create Project Directory

```bash
mkdir nodejs-express-api-basics
cd nodejs-express-api-basics
```

### Step 3: Initialize npm Project

```bash
npm init -y
```

### Step 4: Install Dependencies

For **Basic API** (Express only):
```bash
npm install express
```

For **Enhanced API** (Express + CORS):
```bash
npm install express cors
```

### Step 5: Update package.json

Make sure your `package.json` includes:

```json
{
  "type": "module"
}
```

This enables ES6 module syntax (import/export).

## Basic API (`basic-api.js`)

### Overview

This is the simplest possible Express.js API server. It creates a basic HTTP server with a single health check endpoint. This is a great starting point to verify your server is working correctly.

### Key Concepts

- **Express.js**: A minimal web framework for Node.js that simplifies creating HTTP servers and handling routes.
- **ESM Modules**: Modern JavaScript module system using `import` and `export` instead of CommonJS `require()`.
- **Health Check Endpoint**: A simple endpoint that returns server status, useful for monitoring and testing if the server is running.

### How It Works

1. **Import Express**: The code imports the Express framework.
2. **Create App Instance**: Creates an Express application instance.
3. **Define Route**: Sets up a GET route at `/health` that returns a JSON response with status and timestamp.
4. **Start Server**: The server listens on port 3000 and logs a message when it's ready.

### Usage

1. Save the code as `basic-api.js`
2. Run the server:
   ```bash
   node basic-api.js
   ```
3. You should see: `Server running on http://localhost:3000`

### Testing

1. Start the server: `node basic-api.js`
2. Open your browser or use Insomnia to send a GET request to `http://localhost:3000/health`
3. You should receive a JSON response: `{"status": "ok", "timestamp": "..."}`

### API Endpoint

#### GET /health

**Purpose**: Health check

**URL**: `http://localhost:3000/health`

**Method**: GET

**Response**:
```json
{
  "status": "ok",
  "timestamp": "2024-01-01T12:00:00.000Z"
}
```

## Enhanced API (`server.js`)

### Overview

This enhanced API server adds a POST endpoint to receive and store data, and a GET endpoint to retrieve the stored data. It includes CORS middleware to allow cross-origin requests and JSON parsing middleware to handle JSON request bodies. Data is stored in memory using a variable (in production, you would use a database).

### Key Concepts

- **CORS**: Cross-Origin Resource Sharing middleware that allows the API to accept requests from different origins (domains, ports, or protocols). This is essential when your microcontroller needs to send data to your API from a different origin.
- **JSON Middleware**: The `express.json()` middleware automatically parses incoming JSON request bodies into JavaScript objects, making it easy to access data from POST requests.
- **POST Endpoint**: An endpoint that accepts data in the request body, stores it in a variable, and returns a success response.
- **GET Endpoint**: An endpoint that retrieves and returns the stored data.
- **Data Storage**: Data is stored in a variable (`storedData`) in memory. Note that this data will be lost when the server restarts. In production, use a database.
- **Error Handling**: Try-catch blocks to handle errors gracefully and return appropriate error responses.

### Program Flow

1. **Server Initialization**:
   - Creates Express application instance
   - Sets up middleware (CORS and JSON parsing)
   - Defines endpoints
   - Starts listening on port 3000

2. **Data Storage Variable**:
   - A variable `storedData` is declared at the top level to store received data in memory
   - Initialized as `null` (no data stored initially)

3. **Middleware Setup**:
   - **CORS**: Enables cross-origin requests
   - **JSON Parsing**: Automatically parses JSON request bodies

4. **POST /data Endpoint**:
   - Accepts POST requests with JSON data in the body
   - Stores the received data in the `storedData` variable
   - Logs the data to the console for debugging
   - Returns a success response with the received data

5. **GET /data Endpoint**:
   - Retrieves the data stored in the `storedData` variable
   - Returns a 404 error if no data has been stored yet
   - Returns the stored data in a JSON response if available

6. **Error Handling**:
   - If an error occurs, it's caught and a 500 status code with an error message is returned

7. **Health Check**:
   - The `/health` endpoint remains available for testing server status

### API Endpoints

#### POST /data

**Purpose**: Receive and store data

**URL**: `http://localhost:3000/data`

**Method**: POST

**Request Body** (JSON):
```json
{
  "temperature": 23.5,
  "humidity": 65.2
}
```

**Response** (Success):
```json
{
  "success": true,
  "message": "Data received and stored successfully.",
  "data": {
    "temperature": 23.5,
    "humidity": 65.2
  }
}
```

#### GET /data

**Purpose**: Retrieve stored data

**URL**: `http://localhost:3000/data`

**Method**: GET

**Response** (Success):
```json
{
  "success": true,
  "data": {
    "temperature": 23.5,
    "humidity": 65.2
  }
}
```

**Response** (No Data):
```json
{
  "success": false,
  "message": "No data has been stored yet. Send a POST request to /data first."
}
```

#### GET /health

**Purpose**: Health check

**URL**: `http://localhost:3000/health`

**Method**: GET

**Response**:
```json
{
  "status": "ok",
  "timestamp": "2024-01-01T12:00:00.000Z"
}
```

### Testing with Insomnia

#### Step 1: Start Server

```bash
node server.js
```

#### Step 2: Store Data (POST Request)

1. Open Insomnia (or Postman)
2. Create a new POST request to `http://localhost:3000/data`
3. In the Body tab, select **JSON** format
4. Enter your JSON data:
   ```json
   {
     "temperature": 23.5,
     "humidity": 65.2
   }
   ```
5. Click **Send**
6. You should see a success response confirming the data was stored

#### Step 3: Retrieve Data (GET Request)

1. Create a new GET request to `http://localhost:3000/data`
2. Click **Send**
3. You should receive the data you previously sent via POST

### Testing with cURL

#### Store Data (POST)

```bash
curl -X POST http://localhost:3000/data \
  -H "Content-Type: application/json" \
  -d '{"temperature": 23.5, "humidity": 65.2}'
```

#### Retrieve Data (GET)

```bash
curl http://localhost:3000/data
```

#### Health Check

```bash
curl http://localhost:3000/health
```

## Important Notes

### In-Memory Storage

- Data stored in the variable will be **lost when the server restarts**
- This is fine for testing, but use a database (MongoDB, PostgreSQL, etc.) for production applications
- Consider using a database for persistent storage

### Single Data Item

- The enhanced API stores only **one data item**
- Each new POST request will **overwrite the previous data**
- For multiple items, use an array or a database:
  ```javascript
  let storedData = [];  // Array instead of single value
  
  app.post('/data', (req, res) => {
      storedData.push(req.body);  // Add to array
      res.json({ success: true, data: req.body });
  });
  ```

### CORS Configuration

- The enhanced API allows **all origins** (`cors()` with no options)
- For production, configure CORS to allow only specific origins:
  ```javascript
  app.use(cors({
      origin: 'http://localhost:5173'  // Only allow requests from this origin
  }));
  ```

### Error Handling

- Always use try-catch blocks in async endpoints
- Return appropriate HTTP status codes:
  - `200`: Success
  - `400`: Bad Request (invalid input)
  - `404`: Not Found (resource doesn't exist)
  - `500`: Internal Server Error

## Troubleshooting

### Port Already in Use

**Error**: `Error: listen EADDRINUSE: address already in use :::3000`

**Solution**: Change the PORT number or stop the process using port 3000:
```bash
# Find process using port 3000
lsof -i :3000

# Kill the process
kill -9 <PID>
```

### CORS Errors

**Error**: `Access to fetch at 'http://localhost:3000/data' from origin '...' has been blocked by CORS policy`

**Solution**: Ensure `app.use(cors())` is called before your routes (enhanced API only)

### JSON Parsing Errors

**Error**: `SyntaxError: Unexpected token in JSON at position...`

**Solution**: 
- Ensure request has `Content-Type: application/json` header
- Verify JSON syntax is valid
- Check that `app.use(express.json())` is called before your routes (enhanced API only)

### Module Not Found

**Error**: `Cannot find module 'express'`

**Solution**: Install dependencies:
```bash
npm install express        # For basic API
npm install express cors   # For enhanced API
```

### ES Module Syntax Error

**Error**: `SyntaxError: Cannot use import statement outside a module`

**Solution**: Ensure `package.json` includes:
```json
{
  "type": "module"
}
```

## Customization

### Store Multiple Data Items

Use an array to store multiple items:

```javascript
let storedData = [];

app.post('/data', (req, res) => {
    storedData.push({
        ...req.body,
        timestamp: new Date().toISOString()
    });
    res.json({ success: true, count: storedData.length });
});

app.get('/data', (req, res) => {
    res.json({ success: true, data: storedData });
});
```

### Add Data Validation

Validate incoming data:

```javascript
app.post('/data', (req, res) => {
    const { temperature, humidity } = req.body;
    
    // Validate required fields
    if (temperature === undefined || humidity === undefined) {
        return res.status(400).json({ 
            success: false, 
            error: 'temperature and humidity are required' 
        });
    }
    
    // Validate data types
    if (typeof temperature !== 'number' || typeof humidity !== 'number') {
        return res.status(400).json({ 
            success: false, 
            error: 'temperature and humidity must be numbers' 
        });
    }
    
    storedData = req.body;
    res.json({ success: true, data: storedData });
});
```

### Add Timestamps

Automatically add timestamps to stored data:

```javascript
app.post('/data', (req, res) => {
    storedData = {
        ...req.body,
        timestamp: new Date().toISOString()
    };
    res.json({ success: true, data: storedData });
});
```

### Add DELETE Endpoint

Allow deleting stored data:

```javascript
app.delete('/data', (req, res) => {
    storedData = null;
    res.json({ success: true, message: 'Data deleted' });
});
```

## Next Steps

- **Store multiple data items**: Use an array or database to store multiple entries
- **Add validation**: Ensure incoming data meets your requirements
- **Store in database**: Use MongoDB, PostgreSQL, or another database for persistence
- **Add authentication**: Secure your API endpoints with API keys or JWT tokens
- **Add more endpoints**: Implement PUT, DELETE, or other HTTP methods
- **Implement rate limiting**: Prevent abuse of your API endpoints
- **Add logging**: Use a logging library like Winston or Pino
- **Add environment variables**: Use `.env` files for configuration
- **Add tests**: Write unit and integration tests for your API

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/03-input-and-write/build-your-own-api)

## Additional Resources

- [Express.js Documentation](https://expressjs.com/) - Official Express.js documentation
- [CORS Documentation](https://expressjs.com/en/resources/middleware/cors.html) - CORS middleware documentation
- [Node.js Documentation](https://nodejs.org/docs/) - Node.js official documentation
- [REST API Tutorial](https://www.restapitutorial.com/) - Learn about REST APIs
- [Insomnia](https://insomnia.rest/) - API testing tool

## Notes

- Start with `basic-api.js` to verify your setup works
- Move to `server.js` when you need to receive data from microcontrollers
- CORS is enabled for all origins in enhanced API - configure it properly for production
- Data is stored in memory - will be lost on server restart
- Single data item storage in enhanced API - each POST overwrites previous data
- Error handling is basic - add more robust validation and error handling for production
