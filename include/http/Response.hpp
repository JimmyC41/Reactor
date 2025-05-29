#pragma once

#include <string>
#include <unordered_map>

using U_Map = std::unordered_map<std::string, std::string>;

struct Response
{
    int m_status = 200;
    std::string m_statusText = "OK";
    U_Map m_headers;
    std::string m_body;

    void setStatus(const int& status) { m_status = status; }
    void setStatusText(const std::string& statusText) { m_statusText = statusText; }
    void setHeader(const std::string& key, const std::string& value) { m_headers[key] = value; }
    void setBody(const std::string& body) { m_body = body; }

    // Converts Response struct to a string in HTTP format
    std::string renderString() const;
};