#include <esp_spiffs.h>
#include <esp_log.h>
#include "main.h"
#include "WiFi.h"
#include "CppConsole.h"
#include "ota.h"
#include "CPPNVS.h"
#include "spiffs-ota.h"

using namespace std;

static const char *TAG = CONFIG_AP_MODE_HOSTNAME_PREFIX;

Ota ota;
Console console;
WiFi wifi;
AppFramework framework(wifi);
AppMain appMain;

APP_RUN(appMain)

void AppMain::run()
{
    framework.init();
    console.init(Console::TelnetConsole, [this](auto args){
        this->processCommand(args);
    });
}

void AppMain::processCommand(vector<string> args)
{

}

void AppMain::apActive()
{
    printf("AP active\r\n");
}

void AppMain::apStopped()
{
    printf("AP stopped\r\n");
}

void AppMain::staActive()
{
    printf("STA active\r\n");
}

void AppMain::staStopped()
{
    printf("STA stopped\r\n");
}