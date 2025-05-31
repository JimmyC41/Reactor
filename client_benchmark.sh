#!/bin/sh

sudo sysctl -w net.inet.ip.portrange.hifirst=1024
sudo sysctl -w net.inet.ip.portrange.hilast=65535
sudo sysctl -w net.inet.tcp.msl=3000
sudo sysctl -w net.inet.tcp.fastopen=1

ulimit -n 65536

if ! nc -z localhost 8080; then
    echo "Error: Server not running on localhost:8080"
    exit 1
fi

wrk -t2 -c5k -d30s --latency -H "Connection: close" http://127.0.0.1:8080/hello