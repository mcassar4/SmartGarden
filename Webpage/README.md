# Project Deployment and Management Guide

This guide provides instructions for deploying and managing the frontend and backend servers for this project.

## Table of Contents
- [Project Deployment and Management Guide](#project-deployment-and-management-guide)
  - [Table of Contents](#table-of-contents)
  - [Prerequisites](#prerequisites)
  - [Configuration](#configuration)
    - [IP Addresses](#ip-addresses)
      - [Backend IP Configuration](#backend-ip-configuration)
      - [Frontend IP Configuration](#frontend-ip-configuration)
  - [Deployment](#deployment)
    - [Starting the Servers](#starting-the-servers)
    - [Stopping the Servers](#stopping-the-servers)
    - [Monitoring the Servers](#monitoring-the-servers)
      - [Deployment Logs](#deployment-logs)
      - [Backend Monitoring](#backend-monitoring)
      - [Frontend Monitoring](#frontend-monitoring)

## Prerequisites

Ensure you have the following installed:
- Python 3.x
- Node.js
- npm (Node Package Manager)
- `pscp` (PuTTY Secure Copy client) for Windows

## Configuration

### IP Addresses

Before starting the servers, configure the IP addresses for the frontend and backend.

#### Backend IP Configuration

In `backend/server.js`, set the `IP_ADDRESS` variable to the desired IP address:

```javascript
const IP_ADDRESS = '192.168.1.100'; // Replace with your actual IP address
```

#### Frontend IP Configuration

In `webpage/src/config.js`, set the `IP_ADDRESS` variable to the desired IP address:

## Deployment

To deploy the frontend and backend, run the `upload.bat` script. This script will prompt you for the necessary details and handle the upload process.

### Starting the Servers

To start the servers, run the `start.sh` script. This script will start the backend and frontend servers.

### Stopping the Servers

To stop the servers, run the `stop.sh` script. This script will stop the backend and frontend servers.

### Monitoring the Servers

#### Deployment Logs

Logs for the deployment can be found in the `deployment.log` file. Use the `wipe_logs.sh` script to clear the log files if needed.

#### Backend Monitoring

Logs for the backend can be found in the `backend.log` file. Use the `wipe_logs.sh` script to clear the log files if needed.

#### Frontend Monitoring

Logs for the frontend can be found in the `frontend.log` file. Use the `wipe_logs.sh` script to clear the log files if needed.


