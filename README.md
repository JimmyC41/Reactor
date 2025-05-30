# KQueue HTTP/1.1 Server

A single threaded, KQueue-based (MacOS) HTTP/1.1 server written in C++.

[Link Text](#how-does-it-work?)

## Prerequisites

- C++ compiler with C++23 support
- CMake >= 4.0.2
- Google Test
- Intel TBB

On MacOS with Homebrew:
'''
brew install cmake googletest tbb
'''

## Building

Configure the root level CMakeLists.txt and build.
```
cmake -S . -B build/
cd build
make
```

## Usage

By default, the server cluster listens on port 8080 and serves endpoints.
- GET /hello
- GET /health
- POST /echo

'''
./main
'''

You may wish to increase the kernel's maximum pending TCP connection backlog to 10,240 and the shell's per-process open-file-descriptor limit to 65,536. On MacOs:

'''
sudo sysctl -w kern.ipc.somaxconn=10240
ulimit -n 65536
./main
'''

Then, you may use curl to exercise the API
'''
curl -i http:://localhost::8080/hello

curl -i http:://localhost::8080/health

curl -i \
    -X POST \
    -H "Content-Type: application/json" \
    -d '{"msg":"Optiver"}' \
    http://localhost:8080/echo
'''

## Benchmarking

To benchmark throughput, I used [wrk](https://github.com/wg/wrk), a HTTP benchmarking tool.

To run a benchmark for 10 seconds and keeping 10,000 HTTP concurrent connections:

'''
brew install wrk
wrk -c10k -d10s http://localhost:8080/hello
'''

### How does it work?