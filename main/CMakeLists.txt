# Sources in ui/, e.g. an exported LVGL UI, are picked up automatically
file(GLOB_RECURSE EXTRA_SRCS ui *.c)
idf_component_register(
    SRCS
        "main.cpp"
    ${EXTRA_SRCS}
    INCLUDE_DIRS
        "."
    )
