#pragma once
#include <string>
#include <string_view>
#include <unordered_map>

using u_map = std::unordered_map<std::string, std::string>;

class Request {
private:
    std::string_view    m_method;
    std::string_view    m_path;
    std::string_view    m_version;
    u_map               m_headers;
    std::string         m_body;

public:
    bool parse(std::string_view raw);

    std::string_view method() const { return m_method; }
    std::string_view path() const { return m_path; }
    std::string_view version() const { return m_version; }
    const u_map& headers() const { return m_headers; }
    const std::string& body() const { return m_body; }
};