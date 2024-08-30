const express = require("express");
const cors = require("cors");
const { v4: uuidv4 } = require("uuid");
const axios = require("axios");
const cron = require("node-cron");
const { WebSocketServer } = require("ws");

const app = express();
app.use(cors());
app.use(express.json());

let wateringQueue = []; // In-memory storage for the queue
let espStatus = {}; // In-memory storage for the ESP status

const API_ESP_TX = "http://172.20.10.9/esp_tx"; // Replace with your actual ESP TX endpoint
const API_ESP_RX = "http://172.20.10.9/esp_rx"; // Endpoint for sending commands to ESP

// WebSocket Server for real-time updates
const wss = new WebSocketServer({ noServer: true });

wss.on("connection", (ws) => {
  ws.send(JSON.stringify({ queue: wateringQueue, espStatus }));

  ws.on("message", (message) => {
    // Handle incoming WebSocket messages if necessary
  });
});

const broadcastToClients = () => {
  const payload = JSON.stringify({ queue: wateringQueue, espStatus });
  wss.clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(payload);
    }
  });
};

const fetchWeatherData = async () => {
  try {
    const response = await axios.get("https://api.open-meteo.com/v1/forecast", {
      params: {
        latitude: "43.39141047955725",
        longitude: "-79.76961512140457",
        hourly: "precipitation_probability,temperature_2m",
        current_weather: true,
        timezone: "auto",
        forecast_days: 1, // Only fetch forecast for today
      },
    });
    return response.data;
  } catch (error) {
    console.error("Error fetching weather data:", error);
    return null;
  }
};

const calculateAveragePrecipitation = (data) => {
  const times = data.hourly.time;
  const precipitation = data.hourly.precipitation_probability;
  const today = new Date().toISOString().split("T")[0];

  const timeRanges = [
    { start: 7, end: 11 },
    { start: 13, end: 17 },
    { start: 19, end: 23 },
  ];

  const averages = timeRanges.map(({ start, end }) => {
    const filteredRainData = times
      .map((time, index) => ({
        time,
        precipitation: precipitation[index],
      }))
      .filter((item) => {
        const hour = new Date(item.time).getHours();
        return item.time.startsWith(today) && hour >= start && hour < end;
      });

    if (filteredRainData.length === 0) {
      return null;
    }

    const totalPrecipitation = filteredRainData.reduce(
      (sum, item) => sum + item.precipitation,
      0
    );

    return totalPrecipitation / filteredRainData.length;
  });

  const highestAverage = Math.max(...averages.filter((avg) => avg !== null));

  return highestAverage;
};

const sendWateringCommand = async () => {
  try {
    app.post("/queue", (req, res) => {
      const task = {
        id: uuidv4(), // Assign a unique ID
        zone: "a",
        duration: "15",
        scheduledTime: new Date().toISOString(), // Timestamp for when the task was added
      };
      wateringQueue.push(task);
      sortWateringQueue();
      res.status(201).json(task);
    });
    console.log(`Sent watering command to ESP: `);
  } catch (error) {
    console.error("Error sending command to ESP:", error);
  }
};

const checkAndWaterPlants = async () => {
  const weatherData = await fetchWeatherData();
  if (!weatherData) {
    return;
  }

  const avgPrecipitation = calculateAveragePrecipitation(weatherData);
  if (avgPrecipitation !== null && avgPrecipitation < 40) {
    await sendWateringCommand();
  }
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
    console.error("Error fetching ESP status:", error);
  }
};

// Function to process the watering queue
const processWateringQueue = async () => {
  const now = new Date();
  for (const task of wateringQueue) {
    const scheduledTime = new Date(task.scheduledTime);
    if (scheduledTime <= now) {
      try {
        var command;
        if (task.zone === "Stop") {
          command = "Stop";
          wateringQueue = []; // Clear the queue if the stop command is received
        //   console.log(wateringQueue)
          console.log("Stop command sent to ESP");
        } else {
          command = `${task.zone}:${task.duration}`;
        }
        // await axios.post(API_ESP_RX, command, { timeout: 5000 });
        console.log(`Sent command to ESP: ${command}`);

        // Remove the task from the queue after sending the command
        wateringQueue = wateringQueue.filter((item) => item.id !== task.id);
        broadcastToClients(); // Notify clients of the updated queue
      } catch (error) {
        console.error("Error sending command to ESP:", error);
      }
    }
  }
};

// Poll ESP status every 5 seconds
setInterval(fetchESPStatus, 5000);

// Process the watering queue every second
setInterval(processWateringQueue, 1000);

// Get the combined queue and ESP status
app.get("/queue", (req, res) => {
  res.json({ queue: wateringQueue, espStatus });
});

// Function to sort the wateringQueue by scheduledTime
const sortWateringQueue = () => {
  wateringQueue.sort(
    (a, b) => new Date(a.scheduledTime) - new Date(b.scheduledTime)
  );
};

// Add a new task to the queue
app.post("/queue", (req, res) => {
  const task = { ...req.body, id: uuidv4() }; // Assign a unique ID
  wateringQueue.push(task);
  sortWateringQueue(); // Sort the queue after adding a new task
  res.status(201).json(task);
});

// Remove a task from the queue
app.delete("/queue/:id", (req, res) => {
  const { id } = req.params;
  wateringQueue = wateringQueue.filter((task) => task.id !== id);
  sortWateringQueue(); // Sort the queue after removing a task
  res.status(204).end();
  broadcastToClients(); // Notify all clients of the removed task
});

// Schedule the task to run at 12 AM every night
cron.schedule("0 0 * * *", checkAndWaterPlants);
// checkAndWaterPlants(); // Run the task immediately after starting the server

const server = app.listen(5000, () => {
  console.log(`Server running on port 5000`);
});

server.on("upgrade", (request, socket, head) => {
  wss.handleUpgrade(request, socket, head, (ws) => {
    wss.emit("connection", ws, request);
  });
});
