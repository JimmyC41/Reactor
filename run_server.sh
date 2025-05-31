#!/bin/sh
cd build

ulimit -n 65536 
sudo sysctl -w kern.ipc.somaxconn=16384
sudo sysctl -w net.inet.tcp.msl=1000
sudo sysctl -w net.inet.ip.portrange.hifirst=49152
sudo sysctl -w kern.maxfiles=65536

./main