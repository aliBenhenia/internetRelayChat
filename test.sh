#!/bin/bash

SERVER="10.12.2.4"
PORT=6667
CHANNEL="#Room"
CLIENTCOUNT=500
PASS="lol"

# Function to cleanup and exit
cleanup() {
	echo "Cleaning up and terminating..."
	# Kill all background processes
	pkill -P $$ nc
	exit 1
}

# Trap Ctrl+C and call cleanup function
trap cleanup SIGINT

for ((i=1; i<=CLIENTCOUNT; i++))
do
    (
    NICKNAME="TestUser${i}"
    echo "PASS $PASS"
    sleep 0.25
    echo "NICK $NICKNAME"
    sleep 0.25
    echo "USER o o o o"
    sleep 0.25
    # Add a loop to keep the client connected
    while true; do
        sleep 1
    done
    ) | nc $SERVER $PORT &
    sleep 0.1
done

wait 
echo "Disconnected all clients from the server."
