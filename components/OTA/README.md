# OTA
Simple C++ OTA wrapper for the ESP32

To use, clone this repo as a submodule into your components/ folder.

Create an OTA object, for instance in your main function, like:

OTA ota("https://my.update.server.net/path/to/rom.img");

Then, call at any time using ota.update();
