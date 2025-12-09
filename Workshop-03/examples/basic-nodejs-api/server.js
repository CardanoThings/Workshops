// Import required Node.js packages
import express from 'express';              // Web framework for building API
import cors from 'cors';                   // Enable Cross-Origin Resource Sharing

// Create Express application instance
const app = express();

// Server port number
const PORT = 3000;

// Store received data in memory
// In a production app, you would use a database instead
let storedData = null;

// Middleware: Enable CORS to allow requests from different origins
app.use(cors());

// Middleware: Parse JSON request bodies
app.use(express.json());

// POST endpoint to receive and store data
// URL: http://localhost:3000/data
app.post('/data', async (req, res) => {
    try {
        // Extract data from request body
        const data = req.body;
        
        // Store the data in a variable
        storedData = data;
        
        // Log received data to console for debugging
        console.log('Received and stored data:', data);
        
        // Return success response
        res.json({ 
            success: true, 
            message: 'Data received and stored successfully.',
            data: data
        });
    } catch (error) {
        // Handle errors and return error response
        console.error('Error:', error);
        res.status(500).json({ success: false, error: error.message });
    }
});

// GET endpoint to retrieve stored data
// URL: http://localhost:3000/data
app.get('/data', (req, res) => {
    try {
        if (storedData === null) {
            return res.status(404).json({ 
                success: false, 
                message: 'No data has been stored yet. Send a POST request to /data first.' 
            });
        }
        
        // Return the stored data
        res.json({ 
            success: true, 
            data: storedData 
        });
    } catch (error) {
        // Handle errors and return error response
        console.error('Error:', error);
        res.status(500).json({ success: false, error: error.message });
    }
});

// GET endpoint for health check
// Useful for testing if server is running
// URL: http://localhost:3000/health
app.get('/health', (req, res) => {
    res.json({ status: 'ok', timestamp: new Date().toISOString() });
});

// Start server and listen on specified port
app.listen(PORT, () => {
    console.log(`Server running on http://localhost:${PORT}`);
});

