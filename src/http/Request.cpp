#include <sstream>
#include <iostream>
#include <format>
#include "Request.hpp"

/**
 * We want to translate this raw HTTP input into
 * method, path, version, headers (map), body
 
// Start line
"POST /submit-form HTTP/1.1\r\n"

// Header + empty line
"Host: example.com\r\n"
"User-Agent: curl/7.64.1\r\n"
"Content-Type: application/x-www-form-urlencoded\r\n"
"Content-Length: 27\r\n"
"\r\n"

// Body
"field1=value1&field2=value2"
 */

bool Request::parse(const std::string& raw)
{
    // Split header from body
    auto pos = raw.find("\r\n\r\n");
    if (pos == std::string::npos)
        return false;
    
    std::string headerBlock = raw.substr(0, pos);
    m_body = raw.substr(pos + 4);

    // Read lines from header-block
    std::istringstream ss(headerBlock);
    
    // Get the start line and extract method, path, version
    std::string requestLine;
    if (!std::getline(ss, requestLine))
        return false;

    if (requestLine.back() == '\r')
        requestLine.pop_back();
    
    std::istringstream rl(requestLine);
    if (!(rl >> m_method >> m_path >> m_version))
        return false;

    // Extract header(s)
    std::string line;
    while (std::getline(ss, line))
    {
        if (line.back() == '\r')
            line.pop_back();
        
        if (line.empty())
            break;
        
        auto colon = line.find(':');
        if (colon == std::string::npos)
            continue;
        
        auto key = line.substr(0, colon);
        auto val = line.substr(colon + 1);
        
        // Left trim the space after the ':'
        if (!val.empty() && val.front() == ' ')
            val.erase(0, 1);
        
        m_headers[key] = val;
    }

    // printParsedRequest();
    return true;
}

void Request::printParsedRequest() const
{
    std::cout << "----- Printing parsed raw request -----\n";
    std::cout << std::format(
        "method: {}, path: {}, version: {}, body: {}", 
        method(), path(), version(), body());
    std::cout << "\n";
}