const express = require('express');
const cors = require('cors');
const { v4: uuidv4 } = require('uuid');
const axios = require('axios');
const { WebSocketServer } = require('ws');
// const rateLimit = require('express-rate-limit');

const app = express();
app.use(cors());
app.use(express.json());

let wateringQueue = []; // In-memory storage for the queue
let espStatus = {}; // In-memory storage for the ESP status

const API_ESP_TX = 'http://172.20.10.9/esp_tx'; // Replace with your actual ESP TX endpoint

// WebSocket Server for real-time updates
const wss = new WebSocketServer({ noServer: true });

wss.on('connection', (ws) => {
    ws.send(JSON.stringify({ queue: wateringQueue, espStatus }));

    ws.on('message', (message) => {
        // Handle incoming WebSocket messages if necessary
    });
});

const broadcastToClients = () => {
    const payload = JSON.stringify({ queue: wateringQueue, espStatus });
    wss.clients.forEach(client => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(payload);
        }
    });
};

// // Rate Limiting Middleware
// const limiter = rateLimit({
//     windowMs: 250, // 500ms
//     max: 1, // Limit each IP to 1 request per 500ms
//     message: 'Too many requests, please try again later.'
// });

// app.use(limiter);

// Function to poll ESP status
const fetchESPStatus = async () => {
    try {
        const response = await axios.get(API_ESP_TX, { timeout: 5000 });
        if (response.status === 200) {
            espStatus = response.data;
            broadcastToClients();
        }
    } catch (error) {
        console.error('Error fetching ESP status:', error);
    }
};

// Poll ESP status every 5 seconds
setInterval(fetchESPStatus, 5000);

// Get the combined queue and ESP status
app.get('/queue', (req, res) => {
    res.json({ queue: wateringQueue, espStatus });
});

// Add a new task to the queue
app.post('/queue', (req, res) => {
    const task = { ...req.body, id: uuidv4() }; // Assign a unique ID
    wateringQueue.push(task);
    res.status(201).json(task);
    broadcastToClients(); // Notify all clients of the new task
});

// Remove a task from the queue
app.delete('/queue/:id', (req, res) => {
    const { id } = req.params;
    wateringQueue = wateringQueue.filter(task => task.id !== id);
    res.status(204).end();
    broadcastToClients(); // Notify all clients of the removed task
});

const server = app.listen(5000, () => {
    console.log(`Server running on port 5000`);
});

server.on('upgrade', (request, socket, head) => {
    wss.handleUpgrade(request, socket, head, (ws) => {
        wss.emit('connection', ws, request);
    });
});
