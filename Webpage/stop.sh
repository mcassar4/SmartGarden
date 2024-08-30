#!/bin/bash

PID_FILE="./server.pid"
# Check if the PID file exists
if [ -f "$PID_FILE" ]; then
   # Read the PID from the file
   PID=$(cat "$PID_FILE")
   # Check if the process is running
   if ps -p $PID > /dev/null; then
       echo "Stopping servers with PID: $PID..."
       kill $PID
       # Wait for the process to terminate
       wait $PID 2>/dev/null
       # Remove the PID file
       rm -f "$PID_FILE"
       echo "Servers stopped."
   else
       echo "No running server found with PID: $PID. Removing stale PID file."
       rm -f "$PID_FILE"
   fi
else
   echo "PID file not found. Server might not be running."
fi