#pragma once

#include <string>
#include <unordered_map>

using U_Map = std::unordered_map<std::string, std::string>;

struct Request
{
    std::string m_method;
    std::string m_path;
    std::string m_version;
    U_Map       m_headers;
    std::string m_body;

    std::string method() const { return m_method; }
    std::string path() const { return m_path; }
    std::string version() const { return m_version; }
    std::string body() const { return m_body; }

    // Returns false if raw string is not formatted correctly
    bool parse(const std::string& raw);

    // For testing
    void printParsedRequest() const;
};