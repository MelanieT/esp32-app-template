//
// Created by Melanie on 11/11/2023.
//

#ifndef PICOW_WLAN_SETUP_WEBINTERFACE_APISERVER_H
#define PICOW_WLAN_SETUP_WEBINTERFACE_APISERVER_H


#include "ApiHttpRequest.h"
#include "ApiHttpResponse.h"

class ApiServer
{
public:
    ApiServer();

    static ApiHttpResponse RequestHandler(const char *request);
    static ApiHttpResponse RequestHandler(const ApiHttpRequest& request);
};


#endif //PICOW_WLAN_SETUP_WEBINTERFACE_APISERVER_H
