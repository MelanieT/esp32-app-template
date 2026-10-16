# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly

cmake_minimum_required(VERSION 3.12)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
add_compile_options("-Wno-attributes")
idf_build_set_property(COMPILE_OPTIONS "-w" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-fpermissive" APPEND)
# Rename this for your app, it names the firmware image (build/<name>.bin)
project(app-framework)

# The web interface, e.g. the dist-<product> folder of wifi-configurator
if(CONFIG_USE_SETUP_WEB_INTERFACE OR CONFIG_START_WEBSERVER_IN_STATION_MODE)
    spiffs_create_partition_image(${CONFIG_WEB_INTERFACE_SPIFFS_PARTITION_NAME} ${CONFIG_WEB_INTERFACE_DIR} FLASH_IN_PROJECT)
endif()
