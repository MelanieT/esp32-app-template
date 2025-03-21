# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
function(dump_cmake_variables)
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endfunction()

cmake_minimum_required(VERSION 3.12)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
add_compile_options("-Wno-attributes")
idf_build_set_property(COMPILE_OPTIONS "-w" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-fpermissive" APPEND)
project(filtermeter)

spiffs_create_partition_image(spiffs ${CONFIG_WEB_INTERFACE_DIR} FLASH_IN_PROJECT)
