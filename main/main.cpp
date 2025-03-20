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
Console *console = nullptr;
WiFi wifi;
AppMain appMain;

APP_RUN(appMain)

void AppMain::run()
{
    
}

void AppMain::apActive() {

}
void AppMain::apStopped() {

}
void AppMain::staActive() {

}
void AppMain::staStopped() {

}