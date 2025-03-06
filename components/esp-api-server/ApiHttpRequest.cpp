//
// Created by Melanie on 11/11/2023.
//

#include "ApiHttpRequest.h"

#include <utility>
#include "regex"
#include <sys/param.h>

#define HTTPD_SCRATCH_BUF  MAX(CONFIG_HTTPD_MAX_REQ_HDR_LEN, CONFIG_HTTPD_MAX_URI_LEN)

struct httpd_req_aux {
    struct sock_db *sd;                             /*!< Pointer to socket database */
    char            scratch[HTTPD_SCRATCH_BUF + 1]; /*!< Temporary buffer for our operations (1 byte extra for null termination) */
    size_t          remaining_len;                  /*!< Amount of data remaining to be fetched */
    char           *status;                         /*!< HTTP response's status code */
    char           *content_type;                   /*!< HTTP response's content type */
    bool            first_chunk_sent;               /*!< Used to indicate if first chunk sent */
    unsigned        req_hdrs_count;                 /*!< Count of total headers in request packet */
    unsigned        resp_hdrs_count;                /*!< Count of additional headers in response packet */
    struct resp_hdr {
        const char *field;
        const char *value;
    } *resp_hdrs;                                   /*!< Additional headers in response packet */
    struct http_parser_url url_parse_res;           /*!< URL parsing result, used for retrieving URL elements */
#ifdef CONFIG_HTTPD_WS_SUPPORT
    bool ws_handshake_detect;                       /*!< WebSocket handshake detection flag */
    httpd_ws_type_t ws_type;                        /*!< WebSocket frame type */
    bool ws_final;                                  /*!< WebSocket FIN bit (final frame or not) */
    uint8_t mask_key[4];                            /*!< WebSocket mask key for this payload */
#endif
};

ApiHttpRequest::ApiHttpRequest(httpd_req *request)
{
    m_method = "GET";
    if (request->method == HTTP_POST)
        m_method = "POST";
    else if(request->method == HTTP_PUT)
        m_method = "PUT";

    // URL and parameters
    std::string u = request->uri;
    if (u.find('?') != -1)
    {
        m_url = u.substr(0, u.find('?'));
        std::string params = u.substr(m_url.length() + 1);
        std::istringstream p(params);
        std::string  param;

        while (std::getline(p, param, '&'))
        {
            if (param.find('=') != -1)
            {
                auto name= param.substr(0, param.find('='));
                auto value = param.substr(name.length() + 1);

                std::transform(name.begin(), name.end(), name.begin(),
                               [](unsigned char c){ return std::tolower(c); });

                m_params.emplace_back(name, urlDecode(value));
            }
            else
            {
                m_params.emplace_back(param, "");
            }
        }

    }
    else
    {
        m_url = u;
    }

    // Headers
    // Headers are permanently left in the scratch space. Weird.
    // Logic copied from esp_httpd because it won't give a list of all headers
    auto *aux = static_cast<httpd_req_aux *>(request->aux);
    auto count = aux->req_hdrs_count;
    auto hdr_ptr = aux->scratch;

    while (count--)
    {
        const char *val_ptr = strchr(hdr_ptr, ':');
        if (!val_ptr) {
            break;
        }

        std::string line = hdr_ptr;

        auto name = line.substr(0, line.find(':'));
        auto value = line.substr(name.length() + 1);

        std::transform(name.begin(), name.end(), name.begin(),
                       [](unsigned char c){ return std::tolower(c); });

        value = regex_replace(value, std::regex("^ *"), "");
        m_headers.emplace_back(name, value);

        hdr_ptr += strlen(hdr_ptr);
        while (!(*hdr_ptr))
            hdr_ptr++;
    }
    if (request->content_len)
    {
        auto len  = MIN(request->content_len, 512);
        m_body.insert(0, len, ' ');
        if (httpd_req_recv(request, m_body.data(), len) <= 0)
            m_body = "";
    }
}

ApiHttpRequest::ApiHttpRequest(const char *request)
{
    std::string req(request);

    req = std::regex_replace(req, std::regex("\r"), ""); // Remove CR
    auto head = req.substr(0, req.find("\n\n")); // Find first double LF
    auto m = head.substr(0, head.find('\n'));
    std::string headers;
    if (head.length() >= m.length())
        headers = head.substr(m.length() + 1);

    m_method = m.substr(0, m.find(' '));

    auto firstline = m.substr(m_method.length() + 1);
    auto u = firstline.substr(0, firstline.find(' '));
    if (u.find('?') != -1)
    {
        m_url = u.substr(0, firstline.find('?'));
        std::string params = u.substr(m_url.length() + 1);
        std::istringstream p(params);
        std::string  param;

        while (std::getline(p, param, '&'))
        {
            if (param.find('=') != -1)
            {
                auto name= param.substr(0, param.find('='));
                auto value = param.substr(name.length() + 1);

                std::transform(name.begin(), name.end(), name.begin(),
                               [](unsigned char c){ return std::tolower(c); });

                m_params.emplace_back(name, urlDecode(value));
            }
            else
            {
                m_params.emplace_back(param, "");
            }
        }

    }
    else
    {
        m_url = u;
    }


    m_body = req.length() > head.length() + 2 ? req.substr(head.length() + 2) : "";

    std::istringstream h(headers);
    std::string line;

    while (std::getline(h, line))
    {
        if (line.find(':') != -1)
        {
            auto name = line.substr(0, line.find(':'));
            auto value = line.substr(name.length() + 1);

            std::transform(name.begin(), name.end(), name.begin(),
                           [](unsigned char c){ return std::tolower(c); });

            value = regex_replace(value, std::regex("^ *"), "");
            m_headers.emplace_back(name, value);
        }
    }

}

char ApiHttpRequest::fromHex(char ch) {
    return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

std::string ApiHttpRequest::urlDecode(std::string text) {
    char h;
    std::ostringstream escaped;
    escaped.fill('0');

    for (auto i = text.begin(), n = text.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        if (c == '%') {
            if (i[1] && i[2]) {
                h = fromHex(i[1]) << 4 | fromHex(i[2]);
                escaped << h;
                i += 2;
            }
        } else if (c == '+') {
            escaped << ' ';
        } else {
            escaped << c;
        }
    }

    return escaped.str();
}

std::string ApiHttpRequest::method() const
{
    return m_method;
}

std::string ApiHttpRequest::url() const
{
    return m_url;
}

std::optional<std::string> ApiHttpRequest::param(std::string name)
{
    return getValue(m_params, std::move(name));
}

std::optional<std::string> ApiHttpRequest::header(std::string name)
{
    return getValue(m_headers, std::move(name));
}

//std::list<std::string> ApiHttpRequest::params(std::string name)
//{
//    return std::list<std::string>();
//}
//
//std::list<std::string> ApiHttpRequest::headers(std::string name)
//{
//    return std::list<std::string>();
//}

std::string ApiHttpRequest::body() const
{
    return m_body;
}

std::optional<std::string> ApiHttpRequest::getValue(const std::list <std::pair<std::string, std::string>>& collection, std::string name)
{
    std::transform(name.begin(), name.end(), name.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    for (const auto& item : collection)
    {
        if (item.first == name)
            return {item.second};
    }

    return {};
}

std::string ApiHttpRequest::toString()
{
    std::ostringstream o;

    o << "URL: " << m_url << std::endl;
    o << "Method: " << m_method << std::endl;
    if (!m_headers.empty())
        o << "Headers:" << std::endl;
    for (const auto& h: m_headers)
        o << h.first << "=" << h.second << std::endl;
    if (!m_params.empty())
        o << "Params:" << std::endl;
    for (const auto& h: m_params)
        o << h.first << ": " << h.second << std::endl;

    o << "Body:" << std::endl;
    o << m_body << std::endl;

    return o.str();
}

