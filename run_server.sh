#! /bin/sh

cd build

sudo sysctl -w kern.ipc.somaxconn=10240
ulimit -n 65536

./main