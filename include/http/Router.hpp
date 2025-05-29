#pragma once
#include "Request.hpp"
#include "Response.hpp"

struct Router
{  
    static Response createBadRequest();
    static Response processRequest(const Request& request);
};