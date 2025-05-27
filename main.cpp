#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include "TCPListener.hpp"
#include "TCPStream.hpp"

int main() {
    constexpr int PORT = 8080;
    TCPListener listener{PORT};
    if (listener.start() < 0) {
        std::cerr << "Failed to start listener on port " << PORT << "\n";
        return 1;
    }
    std::cout << "Listening on port " << PORT << "...\n";

    while (true) {
        TCPStream* client = nullptr;
        // wait for a connection
        while (!(client = listener.accept())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "Connected: "
                  << client->getPeerIP() << ":" << client->getPeerPort() << "\n";

        char buffer[1024] = {};
        ssize_t n = client->receive(buffer, sizeof(buffer) - 1);
        if (n > 0) {
            std::cout << "Received:\n" << buffer << "\n";

            // simple HTTP response
            const char* response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: 13\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Hello, world!";
            client->send(const_cast<char*>(response), std::strlen(response));
        }

        delete client;
    }

    return 0;
}