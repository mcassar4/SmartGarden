#!/bin/bash

source ~/.bashrc


# Check if SERVER_PID is set
if [ -z "$SERVER_PID" ]; then
   echo "SERVER_PID is not set. Server might not be running."
   exit 1
fi

# Check if the process is running
if ps -p $SERVER_PID > /dev/null; then
   echo "Stopping servers with PID: $SERVER_PID..."
   kill $SERVER_PID

   # Wait for the process to terminate
   wait $SERVER_PID 2>/dev/null

   # Remove the SERVER_PID variable from ~/.bashrc
   sed -i '/export SERVER_PID/d' ~/.bashrc

   # Source the .bashrc file to clear the variable in the current session
   source ~/.bashrc
   echo "Servers stopped. SERVER_PID removed from ~/.bashrc."
else
   echo "No running server found with PID: $SERVER_PID. Clearing SERVER_PID from ~/.bashrc."

   # Remove the SERVER_PID variable if it was stale
   sed -i '/export SERVER_PID/d' ~/.bashrc
   source ~/.bashrc
fi