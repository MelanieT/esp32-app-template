//
// Created by Melanie on 11/11/2023.
//

#ifndef PICOW_WLAN_SETUP_WEBINTERFACE_HTTPREQUEST_H
#define PICOW_WLAN_SETUP_WEBINTERFACE_HTTPREQUEST_H


#include <list>
#include <string>
#include <optional>
#include "esp_http_server.h"

class ApiHttpRequest
{
public:
    explicit ApiHttpRequest(httpd_req_t *request);
    explicit ApiHttpRequest(const char *request);
    [[nodiscard]] std::string method() const;
    [[nodiscard]] std::string url() const;
    std::optional<std::string> param(std::string name);
//    std::list<std::string> params(std::string name);
    std::optional<std::string> header(std::string name);
//    std::list<std::string> headers(std::string name);
    [[nodiscard]] std::string body() const;
    [[nodiscard]] std::string toString();

private:
    static char fromHex(char ch);
    static std::string urlDecode(std::string text);
    std::string m_method;
    std::string m_url;
    std::list<std::pair<std::string, std::string>> m_params;
    std::list<std::pair<std::string, std::string>> m_headers;
    std::string m_body;
    static std::optional<std::string> getValue(const std::list<std::pair<std::string, std::string>>& collection, std::string name);

};


#endif //PICOW_WLAN_SETUP_WEBINTERFACE_HTTPREQUEST_H
