#include <functional>
#include <unordered_map>
#include <format>
#include "Router.hpp"

namespace
{
    using Handler = std::function<Response(const Request& request)>;
    using HandlerMap = std::unordered_map<std::string, Handler>;

    HandlerMap initRoutes() {
        HandlerMap routes;

        routes["GET /hello"] = [](const Request& req)
        {
            Response res;
            res.setStatus(200);
            res.setStatusText("OK");
            res.setBody("Hello World!");
            res.setHeader("Content-Type", "text/plain");
            return res;
        };

        routes["GET /health"] = [](const Request& req)
        {
            Response res;
            res.setStatus(200);
            res.setStatusText("OK");
            res.setBody(R"({"status":"up"})");
            res.setHeader("Content-Type", "application/json");
            return res;
        };

        routes["POST /echo"] = [](const Request& req) {
            Response res;
            res.setStatus(200);
            res.setStatusText("OK");
            res.setBody(req.body());
            res.setHeader("Content-Type", "application/json");
            return res;
        };

        return routes;
    }
}

Response Router::createBadRequest()
{
    Response response;
    response.setStatus(400);
    response.setStatusText("Bad Request");
    response.setBody("Bad Request");
    response.setHeader("Content-Type", "text/plain");
    return response;
}

Response Router::processRequest(const Request& request)
{
    static auto routes = initRoutes();
    auto key = std::format("{} {}", request.method(), request.path());
    auto it = routes.find(key);

    if (it != routes.end())
        return it->second(request);

    Response res;
    res.setStatus(404);
    res.setStatusText("Not Found");
    return res;
}