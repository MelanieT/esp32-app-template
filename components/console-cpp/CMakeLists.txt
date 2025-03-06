idf_component_register(SRCS "CppConsole.cpp"
    INCLUDE_DIRS ./include
    REQUIRES base_console telnetd-bsd uart-console
    )
