# ESP32-app-template #

A starting point for developing C++ applications for the ESP32 with modern features like web based Wifi configuration, over the air updates and an API server to use as a backend for modern single page web applications written with React or Vue.

### Why use this? ###

Some people like to use the popular Arduino framework, but the IDE it comes with is not everyone's cup of tea. Also, while it provides a number of good solutions for common problems, it's still not a one stop shop to get off the ground. This project aims to be that starting point, offering an uncluttered environment with a lot of functionality.

Much of the code isn't mine, I used snippets from many examples, stack overflow, etc. I also fully integrated the excellent cpp_utils package, which had not been updated to work with ESP IDF version 5.

Credits are given to all authors of the projects, located in repositories of their own that are used by this project.

### Getting started ###

> First, ensure you have an installed and configured ESP-IDF version 5.4 or higher. You will need to have run the IDF's export.sh script in the shell you use to build this project.

These instructions work on MacOS and Linux. I do not use Windows, so I can't point out the specific steps that may be different, but generally speaking, if you follow the ESP IDF installation instructions for Windows and have installed git for Windows and some kind of linux-based shell, as provided by both the git for Windows package (Git Bash) and cygwin, you should be good to go.

Start by changing into your working directory and cloning this repository:

`git clone https://github.com/MelanieT/esp32-app-template.git`

At this point, **do NOT** open the project in an IDE, because an IDE would run cmake on it and we're not ready for that just yet.

Change directory into the downloaded project.

Update the submodules by executing

`git submodule update --init`

After a short wait, the submodules used by this project will be populated.

The next step is to configure the project. Here, you will need to determine which features of the project you don't need.

OTA (over the air update) requires an update server. It is easy to run one on your own computer, using Python's flask web server is probably the easiest way. How to do that is out of scope for this document. However, you need to be able to retrieve both the built firmware and the web interface from a URL in order to use OTA. If you don't have a server, or don't want to use OTA, skip the relevant sections marked **OTA**.

If you wish to use the web based Wifi setup, you will need to download and build a web interface that performs that task. Mine can be found here [https://github.com/MelanieT/wifi-configurator.git](https://github.com/MelanieT/wifi-configurator.git). This is a separate download and build that requires a different toolbox. Refer to the project on how to build it.

It is also possible to OTA update the web application, it can be done using the same server you may already have for the firmware OTA. The web setup is optional, if you don't need it, skip the sections marked **WEB**.

To proceed with configuration, execute

`idf.py menuconfig`

You will be presented with a text mode, menu driven configuration system. Users of Raspberry Pi would be familiar with it, it is quite similar to raspi-config.

![](https://raw.githubusercontent.com/MelanieT/esp32-app-template/refs/heads/master/images/config1.png)

Enter "Component config"

![](https://raw.githubusercontent.com/MelanieT/esp32-app-template/refs/heads/master/images/config2.png)

We will be visiting the components outlined in red.

[**OTA**] Select OTA Firmware OTA component. Enter the URI to load updated firmware from. This will be a http:// address for a server you have previously set up.

You should not need to make any changes in the console-cpp settings and C++ settings menus.

[**WEB**] Select SPIFFS ota component and enter the *base* path of your spiffs update image. The partition name will be appended to this in order to allow updating multiple SPIFFS partitions. Enter `http://your.server.local/filesystems` to create the URL `http://your.server.local/filesystems/spiffs.bin`, where "spiffs" is the partition label.

Finally, enter CPP App Framework

![](https://github.com/MelanieT/esp32-app-template/blob/master/images/config3.png?raw=true)

[**WEB**] Select "Use access point web setup". Set the name of the SPIFFS partition according to your configuration. "spiffs" is the default and must match partitions.csv. Enter the directory for creating the SPIFFS content, which will be the "dist" or "build" folder of the web frontend project that should be built already. Set a password, if desired, to secure your configuration screen. If your application will use this framework's web server to serve a web interface when connected to Wifi, select the option to start the webserver on wifi. You should not need to change the SPIFFS base path unless you're starting the web server from your own code.

If you selected to run the web server on wifi connections, you can also enable the device control API to be accessible when running on wifi.
> #### WARNING ####
> The device control API is a powerful API that can be used to change the wifi parameters. It is **NOT** protected by a password! Only use in a secured environment, if you need it. Don't enable this without a clear need!

Finally, set the hostname prefix. The hostname prefix will be used to generate a hostname of the form prefix-XXXXXX.local, and also becomes the name of the access point, if configured.

Exit the components menu and select Serial flasher config. Set the flash size of your device. 4MB is the minimum supported by this project. If your flash size is larger, you may want to modify partitions.csv to take advantage of the additional space.

Now, run

`idf.py build`

If all goes well, you will now have a firmware image. This is the moment to open the project in the IDE of your choice. As with all ESP32 projects, you will need to select a toolchain and a build directory. In order to be compatible with ESP IDF, the build directory **MUST** be named "build".

You can now edit and expand main/main.cpp to create your application functionality.

#### Happy hacking ####