//
// Created by Melanie on 11/11/2023.
//

#include <sstream>
#include <utility>
#include "ApiHttpResponse.h"
#include "HttpStatus.h"

ApiHttpResponse::ApiHttpResponse()
{
    m_headers.emplace_back(std::pair("Content-Type", "application/json"));
    m_headers.emplace_back(std::pair("Connection", "close"));
}


std::string ApiHttpResponse::ToString(int statusCode)
{
    if (statusCode)
        m_statusCode = statusCode;

    std::ostringstream response;

    response << "HTTP/1.1 " << m_statusCode << " " << HttpStatus::reasonPhrase(m_statusCode) << "\r\n";
    if (!m_body.empty())
        AddHeader("Content-Length", std::to_string(m_body.length()));

    for (const auto& h : m_headers)
        response << h.first << ": " << h.second << "\r\n";

    response << "\r\n"; // End of headers
    response << m_body;
    response.flush();

    return response.str();
}

void ApiHttpResponse::AddHeader(const std::string& header, const std::string& value)
{
    if (header != "Accept") // Allow only Accept to stack
    {
        m_headers.remove_if([=] (auto p) { return p.first == header; });
    }
    m_headers.emplace_back(header, value);
}

void ApiHttpResponse::setStatusCode(HttpStatus::Code code)
{
    m_statusCode = toInt(code);
}

void ApiHttpResponse::setBody(std::string body)
{
    m_body = std::move(body);
}

void ApiHttpResponse::appendBody(const std::string& body)
{
    m_body += body;
}

esp_err_t ApiHttpResponse::sendResponse(httpd_req_t *request)
{
    std::string status = std::to_string(m_statusCode) + " " + HttpStatus::reasonPhrase(m_statusCode);
    httpd_resp_set_status(request, status.data());

    for (const auto &h: m_headers)
        httpd_resp_set_hdr(request, h.first.data(), h.second.data());

    return httpd_resp_send(request, m_body.data(), m_body.length());
}

