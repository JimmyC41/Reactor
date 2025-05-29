#include "Router.hpp"

Response Router::createBadRequest()
{
    Response response;
    response.setStatus(400);
    response.setStatusText("Bad Request");
    response.setBody("Bad Request");
    response.setHeader("Content-Type","text/plain");
    return response;
}

Response Router:: processRequest(const Request& request)
{
    Response response;

    if (request.method() == "GET" && request.path() == "/hello")
    {
        response.setStatus(200);
        response.setStatusText("Ok");
        response.setBody("Hello World!");
        response.setHeader("Content-Type", "text/plain");
    }
    else if (request.method() == "GET" && request.path() == "/health")
    {
        response.setStatus(200);
        response.setStatusText("Ok");
        response.setBody(R"({"status": "up})");
        response.setHeader("Content-Type", "application/json");
    }
    else if (request.method() == "POST" && request.path() == "/echo")
    {
        response.setStatus(200);
        response.setStatusText("Ok");
        response.setBody(request.body());
        response.setHeader("Content-Type", "application/json");
    }
    else
    {
        response.setStatus(404);
        response.setStatusText("Not Found");
        response.setHeader("Content-Type","text/plain");
    }

    return response;
}