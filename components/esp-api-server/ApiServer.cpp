//
// Created by Melanie on 11/11/2023.
//

#include "ApiServer.h"
#include "ApiHttpResponse.h"
#include "ApiHttpRequest.h"
#include "UrlMapper.h"
#include <string>
#include <regex>

ApiServer::ApiServer()
= default;

ApiHttpResponse ApiServer::RequestHandler(const char *request)
{
    ApiHttpRequest req(request);

    ApiHttpResponse response;

    UrlMapper::Map(req, response);

    return response;
}

ApiHttpResponse ApiServer::RequestHandler(const ApiHttpRequest &request)
{
    ApiHttpResponse response;

    UrlMapper::Map(request, response);

    return response;
}
