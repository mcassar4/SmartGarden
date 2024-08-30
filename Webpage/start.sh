#!/bin/bash

DEPLOY_SCRIPT_PATH="./server.py"
LOG_FILE="./deployment.log"
PID_FILE="./server.pid"

# Run the deploy script with nohup
echo "Starting servers with nohup..."
nohup python3 "$DEPLOY_SCRIPT_PATH" > "$LOG_FILE" 2>&1 &

# Get the process ID of the last background command, save it to a file
PID=$!
echo $PID > "$PID_FILE"

# Output the PID to the console for reference
echo "Servers started with PID: $PID"
echo "Logs can be found in: $LOG_FILE"
echo "PID saved in: $PID_FILE"