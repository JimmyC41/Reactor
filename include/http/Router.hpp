#pragma once
#include "Request.hpp"
#include "Response.hpp"

class Router
{
public:
    static Response processRequest(const Request& request);
    static Response createBadRequest();
};