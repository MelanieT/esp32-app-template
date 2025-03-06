//
// Created by Melanie on 11/11/2023.
//

#ifndef PICOW_WLAN_SETUP_WEBINTERFACE_HTTPRESPONSE_H
#define PICOW_WLAN_SETUP_WEBINTERFACE_HTTPRESPONSE_H

#include <string>
#include <list>
#include <esp_err.h>
#include <esp_http_server.h>
#include "HttpStatus.h"

class ApiHttpResponse
{
public:
    ApiHttpResponse();

    void AddHeader(const std::string& header, const std::string& value);
    void setStatusCode(HttpStatus::Code);
    void setBody(std::string body);
    void appendBody(const std::string& body);
    std::string ToString(int statusCode = 0);
    esp_err_t sendResponse(httpd_req_t *request);
    inline int statusCode() const { return m_statusCode; };

private:
    std::list<std::pair<std::string, std::string>> m_headers;
    int m_statusCode = 200;
    std::string m_body;
};


#endif //PICOW_WLAN_SETUP_WEBINTERFACE_HTTPRESPONSE_H
