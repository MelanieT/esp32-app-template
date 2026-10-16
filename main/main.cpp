#include <esp_log.h>
#include "main.h"
#include "WiFi.h"
#include "CppConsole.h"
#include "ota.h"
#include "spiffs-ota.h"
#include "console.h"

using namespace std;

static const char *TAG = CONFIG_AP_MODE_HOSTNAME_PREFIX;

Ota ota;
Console console;
WiFi wifi;
AppFramework framework(&wifi);
AppMain appMain;

APP_RUN(appMain)

void AppMain::run()
{
    framework.setHandler(this);
    framework.setWebAppIsSpa(true);
    framework.init();

    console.init(Console::TelnetConsole, [this](auto args){
        this->processCommand(args);
    });

    // Start your app here

    vTaskDelete(nullptr);
}

void AppMain::processCommand(vector<string> args) // NOLINT
{
    if (args.empty())
        return;

    if (args[0] == "help")
    {
        console_printf("info        Hostname, ip address and network\r\n");
        console_printf("update      Update web interface and firmware (CONFIG_SPIFFS_OTA_URI, CONFIG_OTA_SERVER_URI)\r\n");
        console_printf("forgetwifi  Erase the wifi credentials and restart into setup\r\n");
        console_printf("reboot      Restart\r\n");
    }
    else if (args[0] == "info")
    {
        console_printf("Hostname %s, ip %s, station %s, access point %s\r\n", framework.hostname().c_str(),
                       framework.ipAddress().c_str(), framework.stationActive() ? "up" : "down",
                       framework.apActive() ? "up" : "down");
    }
    else if (args[0] == "update")
    {
        console_printf("Updating SPIFFS\r\n");
        ota_spiffs("spiffs");

        console_printf("Updating firmware\r\n");
        ota.update(); // Does not return
    }
    else if (args[0] == "forgetwifi")
    {
        framework.forgetWifi(); // Does not return
    }
    else if (args[0] == "reboot")
    {
        esp_restart();
    }
}

// Called from the wifi event task: return quickly, start longer work in a task of its own
void AppMain::staActive()
{
    ESP_LOGI(TAG, "On the network as %s.local, ip %s", framework.hostname().c_str(), framework.ipAddress().c_str());
}

void AppMain::staStopped()
{
}
