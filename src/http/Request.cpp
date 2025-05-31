#include <sstream>
#include <iostream>
#include <format>
#include <cctype>
#include <algorithm>
#include "spdlog/spdlog.h"
#include "Request.hpp"

// TODO: Refactor long method
// Populate Request struct given raw bytes

bool Request::parse(std::string_view raw)
{
    m_headers.clear();
    m_body.clear();
    m_headers.reserve(16);
    m_body.reserve(1024);

    // Check for empty or whitespace only input
    bool isEmpty = true;
    for (char c : raw)
    {
        if (!std::isspace(c))
        {
            isEmpty = false;
            break;
        }
    }

    if (isEmpty)
        return false;

    // Find header-body separator
    auto pos = raw.find("\r\n\r\n");
    std::string_view headerBlock = (pos == std::string_view::npos)
        ? raw : raw.substr(0, pos);
    
    if (pos != std::string_view::npos)
        m_body = std::string(raw.substr(pos + 4));

    // Parse request line
    auto requestLineEnd = headerBlock.find("\r\n");
    std::string_view requestLine = (requestLineEnd == std::string_view::npos)
        ? headerBlock : headerBlock.substr(0, requestLineEnd);

    // Trim whitespace from request line
    while (!requestLine.empty() && std::isspace(requestLine.front()))
        requestLine.remove_prefix(1);
    while (!requestLine.empty() && std::isspace(requestLine.back()))
        requestLine.remove_suffix(1);

    if (requestLine.empty())
        return false;

    // Parse method
    size_t start = 0, end = requestLine.find(' ');
    if (end == std::string_view::npos)
        return false;
    
    m_method = requestLine.substr(0, end);

    // Parse path
    start = end + 1;
    end = requestLine.find(' ', start);
    if (end == std::string_view::npos)
        return false;
    
    m_path = requestLine.substr(start, end - start);

    // Parse version
    m_version = requestLine.substr(end + 1);

    // Process headers (if any)
    if (requestLineEnd != std::string_view::npos)
    {
        start = requestLineEnd + 2;
        while (start < headerBlock.size())
        {
            end = headerBlock.find("\r\n", start);
            if (end == std::string_view::npos || end == start)
                break;

            std::string_view line = headerBlock.substr(start, end - start);
            auto colon = line.find(':');
            if (colon == std::string_view::npos)
            {
                start = end + 2;
                continue;
            }

            std::string_view key = line.substr(0, colon);
            std::string_view val = line.substr(colon + 1);
            if (!val.empty() && val.front() == ' ')
                val.remove_prefix(1);

            m_headers[std::string(key)] = std::string(val);
            start = end + 2;
        }
    }
    return true;
}