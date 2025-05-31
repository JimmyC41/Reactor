#pragma once
#include <string>
#include <unordered_map>

using u_map = std::unordered_map<std::string, std::string>;

class Response {
private:
    int         m_status;
    std::string m_statusText;
    std::string m_body;
    u_map       m_headers;

public:
    Response() : m_status(200), m_statusText("OK") {}

    void setStatus(int status) { m_status = status; }
    void setStatusText(const std::string& text) { m_statusText = text; }
    void setBody(const std::string& body) { m_body = body; }
    void setHeader(const std::string& key, const std::string& value) { m_headers[key] = value; }

    std::string renderString() const;
};