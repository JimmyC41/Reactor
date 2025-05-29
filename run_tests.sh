#! /bin/sh

cd build

# Temporarily increase how many fds the current process can open
ulimit -n 65536

./test/UnitTests