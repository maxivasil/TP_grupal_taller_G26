#!/bin/bash
cd /home/machi/AA_FIUBA/Taller/TP_grupal_taller_G26/build

# Kill any existing processes
pkill -f taller_server || true
pkill -f taller_client || true
sleep 1

# Start server
echo "[*] Starting server on port 80000..."
./taller_server 80000 > /tmp/server.log 2>&1 &
SERVER_PID=$!

sleep 2

# Start client
echo "[*] Starting client..."
./taller_client localhost 80000

# Kill server when done
kill $SERVER_PID 2>/dev/null || true
