const express = require('express');
const cors = require('cors');
const { v4: uuidv4 } = require('uuid');
const axios = require('axios');
const { WebSocketServer } = require('ws');

const app = express();
app.use(cors());
app.use(express.json());

let wateringQueue = []; // In-memory storage for the queue
let espStatus = {}; // In-memory storage for the ESP status

const API_ESP_TX = 'http://172.20.10.9/esp_tx'; // Replace with your actual ESP TX endpoint
const API_ESP_RX = 'http://172.20.10.9/esp_rx'; // Endpoint for sending commands to ESP

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

// Function to process the watering queue
const processWateringQueue = async () => {
    const now = new Date();
    for (const task of wateringQueue) {
        const scheduledTime = new Date(task.scheduledTime);
        if (scheduledTime <= now) {
            try {
                const command = `${task.zone}:${task.duration}`;
                await axios.post(API_ESP_RX, command, { timeout: 5000 });
                console.log(`Sent command to ESP: ${command}`);
                
                // Remove the task from the queue after sending the command
                wateringQueue = wateringQueue.filter(item => item.id !== task.id);
                broadcastToClients(); // Notify clients of the updated queue
            } catch (error) {
                console.error('Error sending command to ESP:', error);
            }
        }
    }
};

// Poll ESP status every 5 seconds
setInterval(fetchESPStatus, 5000);

// Process the watering queue every second
setInterval(processWateringQueue, 1000);

// Get the combined queue and ESP status
app.get('/queue', (req, res) => {
    res.json({ queue: wateringQueue, espStatus });
});

// Function to sort the wateringQueue by scheduledTime
const sortWateringQueue = () => {
    wateringQueue.sort((a, b) => new Date(a.scheduledTime) - new Date(b.scheduledTime));
};

// Add a new task to the queue
app.post('/queue', (req, res) => {
    const task = { ...req.body, id: uuidv4() }; // Assign a unique ID
    wateringQueue.push(task);
    sortWateringQueue(); // Sort the queue after adding a new task
    res.status(201).json(task);
    broadcastToClients(); // Notify all clients of the new task
});

// Remove a task from the queue
app.delete('/queue/:id', (req, res) => {
    const { id } = req.params;
    wateringQueue = wateringQueue.filter(task => task.id !== id);
    sortWateringQueue(); // Sort the queue after removing a task
    res.status(204).end();
    broadcastToClients(); // Notify all clients of the removed task
});

const IP_ADDRESS = '192.168.1.100'; // Replace with the IP address you want to bind to
const PORT = 5000;

const server = app.listen(PORT, IP_ADDRESS, () => {
    console.log(`Server running on http://${IP_ADDRESS}:${PORT}`);
});

server.on('upgrade', (request, socket, head) => {
    wss.handleUpgrade(request, socket, head, (ws) => {
        wss.emit('connection', ws, request);
    });
});
