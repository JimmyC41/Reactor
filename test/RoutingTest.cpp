#include <gtest/gtest.h>
#include "Router.hpp"
#include "Request.hpp"
#include "Response.hpp"

using ::testing::Test;
using str = std::string;

class RoutingTest : public Test
{
protected:
    Request request;
    Response response;

    // Parse raw string, process request, render the response
    std::string getResponse(const std::string& raw)
    {
        request.parse(raw);
        response = Router::processRequest(request);
        return response.renderString();
    }
};

// Validate the raw string was correctly parsed into the Request struct

void validateRequest(const Request& request, str method, str path, str ver, str body)
{
    EXPECT_EQ(request.method(), method);
    EXPECT_EQ(request.path(), path);
    EXPECT_EQ(request.version(), ver);
    EXPECT_EQ(request.body(), body);
}

// Validate the router prepared a valid HTTP response to the respective request above

void validateRenderedResponse(const str& response, const str& expected)
{
    EXPECT_EQ(response, expected);
}

TEST_F(RoutingTest, GetHello)
{
    std::string raw = 
        "GET /hello HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n";
    
    std::string reqBody = "Hello World!";
    std::string expected = 
        "HTTP/1.1 200 Ok\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " + std::to_string(reqBody.size()) + "\r\n"
        "Connection: close\r\n\r\n" + 
        reqBody;
    
    auto httpResponse = getResponse(raw);
    validateRequest(request, "GET", "/hello", "HTTP/1.1", "");
    validateRenderedResponse(httpResponse, expected);
};

TEST_F(RoutingTest, GetHealth)
{
    std::string raw = 
        "GET /health HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n";
    
    std::string resBody = R"({"status": "up"})";
    std::string expected = 
        "HTTP/1.1 200 Ok\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(resBody.size()) + "\r\n"
        "Connection: close\r\n\r\n"+
        resBody;
    
    auto httpResponse = getResponse(raw);
    validateRequest(request, "GET", "/health", "HTTP/1.1", "");
    validateRenderedResponse(httpResponse, expected);
};

TEST_F(RoutingTest, PostEcho)
{
    std::string reqBody = R"({"msg":"Optiver Final Round of Doom"})";
    std::string raw =
        "POST /echo HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(reqBody.size()) + "\r\n"
        "\r\n" +
        reqBody;

    std::string expected =
        "HTTP/1.1 200 Ok\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(reqBody.size()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" +
        reqBody;
    
    auto httpResponse = getResponse(raw);
    validateRequest(request, "POST", "/echo", "HTTP/1.1", reqBody);
    validateRenderedResponse(httpResponse, expected);
};