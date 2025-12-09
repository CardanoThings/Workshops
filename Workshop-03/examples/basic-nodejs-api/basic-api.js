// Import required Node.js packages
import express from 'express';              // Web framework for building API

// Create Express application instance
const app = express();

// Server port number
const PORT = 3000;

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

