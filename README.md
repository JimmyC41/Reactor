# KQueue HTTP/1.1 Server

A simple, single threaded, KQueue-based (MacOS) HTTP/1.1 server written in C++.

[Link Text](#how-does-it-work?)

### Prerequisites

- C++ compiler with C++23 support
- CMake >= 4.0.2
- Google Test
- Intel TBB

On MacOS with Homebrew:
'''
brew install cmake googletest tbb
'''

### Building

Configure the root level CMakeLists.txt, build, run the main executable.
```
cmake -S . -B build/
cd build
make
./main
```

### Usage

By default, the server cluster listens on port 8080 and serves endpoints
- GET /hello
- GET /health
- POST /echo

On MacOS, you can use curl to exercise the API:

### Stress Test

To benchmark throughput, I used [wrk](https://github.com/wg/wrk), a HTTP benchmarking tool.

To run a benchmark for 10 seconds and keeping 10,000 HTTP concurrent connections:

'''
brew install wrk
wrk -c10k -d10s http://localhost:8080/hello
'''

### How does it work?

### Design Choices on Parallel Processing