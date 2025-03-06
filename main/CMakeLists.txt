file(GLOB_RECURSE EXTRA_SRCS ui *.c)
idf_component_register(
    SRCS
        "main.cpp"
        "EventHandler.cpp"
        "EventHandler.h"
        "captdns.c"
        "captdns.h"
        "SimpleWebServer.cpp"
        "SimpleWebServer.h"
    ${EXTRA_SRCS}
    INCLUDE_DIRS
        "."
    )

