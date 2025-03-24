#include <esp_spiffs.h>
#include <esp_log.h>
#include "main.h"
#include "WiFi.h"
#include "CppConsole.h"
#include "ota.h"
#include "CPPNVS.h"
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

    vTaskDelete(nullptr);
}

void AppMain::processCommand(vector<string> args) // NOLINT
{
    if (args.empty())
        return;

    if (args[0] == "update")
    {
        console_printf("Updating SPIFFS\r\n");
        ota_spiffs("spiffs");

        console_printf("Updating firmware\r\n");
        ota.update(); // Does not return
    }
}

void AppMain::staActive()
{
}

void AppMain::staStopped()
{
}