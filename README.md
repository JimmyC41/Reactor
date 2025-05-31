# KQueue HTTP/1.1 Server

A single threaded, KQueue-based (MacOS) HTTP/1.1 server written in C++.

## Prerequisites

- C++ compiler with C++23 support
- CMake Version 4.0.2
- Google Test
- spdlog

On MacOS with Homebrew:
```
brew install cmake googletest tbb
```

## Building and Usage

Configure the root level CMakeLists.txt and build.
```
cmake -S . -B build/
cd build
make
./main
```

You can use curl to exercise the API. By default, the server listens on port 8080 and serves endpoints "GET /HELLO", "GET /HEALTH" and "POST /ECHO".

```
curl -i http:://localhost::8080/hello

curl -i http:://localhost::8080/health

curl -i \
    -X POST \
    -H "Content-Type: application/json" \
    -d '{"msg":"Optiver"}' \
    http://localhost:8080/echo
```

## Benchmarking

To benchmark throughput, I used [wrk](https://github.com/wg/wrk), a HTTP benchmarking tool.

To run a benchmark for 30 seconds and keeping 5,000 HTTP concurrent connections:

'''
brew install wrk
wrk -c5k -d30s http://localhost:8080/hello
'''

Alternatively, you can use the sh script provided.

'''
./client_benchmark.sh
'''

On my Macbook (M4 Max, November 2024), the server supported 5K concurrent connections at 12K+ queries-per-second (QPS) and an average end-to-end latency of 10ms.

## Logging

For debugging and error logs, I used [spdlog](https://github.com/gabime/spdlog), a fast C++ logging library. Log files can be found under build/logs/server.log

### How does it work?