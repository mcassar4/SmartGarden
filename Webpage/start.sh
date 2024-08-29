#!/bin/bash

# Define paths
DEPLOY_SCRIPT_PATH="./server.py"
LOG_FILE="./deployment.log"

source ~/.bashrc
# Check if SERVER_PID is set
if [ -z "$SERVER_PID" ]; then
    # Run the deploy script with nohup
    echo "Starting servers with nohup..."
    nohup python3 "$DEPLOY_SCRIPT_PATH" > "$LOG_FILE" 2>&1 &

    # Get the process ID of the last background command
    SERVER_PID=$!

    # Set the environment variable globally in .bashrc (or use .bash_profile, /etc/environment if needed)
    echo "export SERVER_PID=$SERVER_PID" >> ~/.bashrc

    # Source the .bashrc file to make the variable immediately available in the current session
    source ~/.bashrc

    # Output the PID to the console for reference
    echo "Servers started with PID: $SERVER_PID"
    echo "Logs can be found in: $LOG_FILE"
    echo "SERVER_PID set globally in ~/.bashrc"
else
   echo "SERVER_PID is set. Server might be running."
fi

