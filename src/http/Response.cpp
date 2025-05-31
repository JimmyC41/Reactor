#include <sstream>
#include <iostream>
#include <vector>
#include "Response.hpp"

std::string Response::renderString() const
{
    std::vector<char> buffer;
    buffer.reserve(256 + m_body.size());

    auto append = [&buffer](const char* str, size_t len)
    {
        buffer.insert(buffer.end(), str, str + len);
    };

    auto statusLine = std::format("HTTP/1.1 {} {}\r\n", m_status, m_statusText);
    append(statusLine.c_str(), statusLine.size());

    for (const auto& [key, value] : m_headers)
    {
        auto header = std::format("{}: {}\r\n", key, value);
        append(header.c_str(), header.size());
    }

    auto len = std::format("Content-Length: {}\r\n", m_body.size());
    append(len.c_str(), len.size());

    auto close = std::format("Connection: close\r\n\r\n");
    append(close.c_str(), close.size());

    append(m_body.c_str(), m_body.size());

    return std::string(buffer.begin(), buffer.end());
}