#include "Response.hpp"
#include <sstream>
#include <iostream>

std::string Response::renderString() const
{
    std::ostringstream oss;

    // Start line
    oss << "HTTP/1.1 " << m_status << ' ' << m_statusText << "\r\n";

    // Headers
    for (auto& [key, value] : m_headers)
        oss << key << ": " << value << "\r\n";
    oss << "Content-Length: " << m_body.size() << "\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";

    // Body
    oss << m_body << '\n';

    // std::cout << "----- Printing Response ------\n" << oss.str() << "\n";
    return oss.str();
}