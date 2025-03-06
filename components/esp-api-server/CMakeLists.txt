idf_component_register(SRCS ApiServer.cpp
                            ApiHttpRequest.cpp
                            ApiHttpResponse.cpp
                            UrlMapper.cpp
                    INCLUDE_DIRS .
                    REQUIRES esp_http_server
                    )
