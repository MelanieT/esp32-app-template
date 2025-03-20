#include <esp_spiffs.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_mac.h>
#include "main.h"
#include "WiFi.h"
#include "CppConsole.h"
#include "ota.h"
#include "SimpleWebServer.h"
#include "CPPNVS.h"
#include "EventHandler.h"
#include "spiffs-ota.h"
#include "captdns.h"
#include "SetupApi.h"

using namespace std;

static const char *TAG = "filtermeter";

Ota ota;
Console *console = nullptr;
WiFi wifi;
AppMain appMain;
CaptiveDns captDns;

void AppMain::processCommand(vector<string> args) // Do not move, do not pass by ref, we WANT a copy
{
    if (args.empty())
        return;

    if (args[0] == "update")
    {
        SimpleWebServer::stop_webserver(m_webserver);

        spiffsUpdate((m_otaServer + "/spiffs.bin").c_str());
        firmwareUpdate((m_otaServer + "/rom.img").c_str());
    }

    if (args[0] == "spiffsupdate")
    {
        spiffsUpdate((m_otaServer + "/spiffs.bin").c_str());
        esp_restart();
    }

    if (args[0] == "reboot")
    {
        esp_restart();
    }
}

extern "C" {
void app_main()
{
    appMain.run();
}
}

void AppMain::run()
{
    m_otaServer = "http://192.168.201.50/filtermeter/filtermeter";

    NVS nvs("ls");

    mountStorage("/data");

    SetupApi::init();

    if (nvs.get("ssid", &m_ssid))
    {
        m_ssid.clear();
    }
    if (nvs.get("password", &m_password))
    {
        m_password.clear();
    }

    printf("ssid %s\r\n", m_ssid.c_str());

    m_hostname = generateHostname(TAG);

    wifi.setStationHostname(m_hostname);
    wifi.setWifiEventHandler(new EventHandler());

    if (!m_ssid.empty() && !m_password.empty())
    {
        wifi.connectSTA(m_ssid, m_password, false);

        static MDNS mdns;
        mdns.setHostname(m_hostname);
        mdns.serviceAdd(nullptr, "_http", "_tcp", 80);
        mdns.serviceInstanceSet("_http", "_tcp", "_server");
    }
    else
    {
        enterApMode();
    }

    vTaskDelete(nullptr);
}

void AppMain::enterApMode()
{
    if (m_apMode)
        return;

    printf("Entering AP mode\r\n");

    m_apMode = true;

    wifi.disconnectSTA();
    wifi.startAP(generateHostname(TAG), "", WIFI_AUTH_OPEN);

    m_stationConnected = false;

    if (!m_ssid.empty() && !m_password.empty() && !m_stationModeRetryTimer)
    {
        m_stationModeRetryTimer = xTimerCreate("StationRetry", 1800000 / portTICK_PERIOD_MS, pdFALSE, this,
                                               retryStationModeThunk);
        xTimerStart(m_stationModeRetryTimer, 500 / portTICK_PERIOD_MS);
    }
}

void AppMain::retryStationModeThunk(TimerHandle_t handle)
{
    void *data = pvTimerGetTimerID(handle);
    ((AppMain *)data)->retryStationMode(handle);
}

void AppMain::retryStationMode(TimerHandle_t handle) const
{
    if (!m_stationConnected)
        esp_restart();

//    printf("Retry timer\n");
//
//    tryConnectWifi(m_ssid, m_password);
//
//    if (m_stationConnected)
//    {
//        wifi.stopAP();
//
//        xTimerDelete(handle, 500 / portTICK_PERIOD_MS);
//        m_stationModeRetryTimer = nullptr;
//    }
}

void AppMain::apStarted()
{
    captDns.start();
    if (!m_webserver)
        m_webserver = SimpleWebServer::start_webserver("/data");
}

void AppMain::apStoppedOld()
{
    if (m_webserver)
        SimpleWebServer::stop_webserver(m_webserver);
    m_webserver = nullptr;

    captDns.stop();
}


esp_err_t AppMain::unmountStorage()
{
    esp_vfs_spiffs_unregister("spiffs");
}

esp_err_t AppMain::mountStorage(const char *basePath)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = basePath,
        .partition_label = "spiffs",
        .max_files = 5,   // This sets the maximum number of files that can be open at the same time
        .format_if_mount_failed = false
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info("spiffs", &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    return ESP_OK;
}

string AppMain::generateHostname(const string& hostname_base)
{
    uint8_t chipid[6];
    esp_read_mac(chipid, ESP_MAC_WIFI_STA);
    static char hostname[32];
    snprintf(hostname, 32, "%s_%02x%02x%02x", hostname_base.c_str(), chipid[3], chipid[4], chipid[5]);

    return {hostname};
}

void AppMain::tryConnectWifi(std::string ssid, std::string password)
{
    if (m_testConnection)
        return;

    m_testConnection = true;

    printf("Attempting to connect\r\n");
    auto ret = wifi.connectSTA(ssid, password, true);
    printf("Connect done, result %d\r\n", ret);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    m_testConnection = false;

    if (ret)
    {
        wifi.disconnectSTA();
        return;
    }
    m_stationConnected = true;

    if (m_stationModeRetryTimer)
    {
        xTimerDelete(m_stationModeRetryTimer, 500 / portTICK_PERIOD_MS);
        m_stationModeRetryTimer = nullptr;
    }

    NVS nvs("ls");

    nvs.set("ssid", ssid);
    nvs.set("password", password);

    nvs.commit();
}

void AppMain::spiffsUpdate(const char *from)
{
    ota_spiffs(from);
}

void AppMain::firmwareUpdate(const char *from)
{
    vTaskDelay(100 / portTICK_PERIOD_MS);

    ota.update("http://192.168.201.50/lightswitch/lightswitch/rom.img");

    esp_restart();
}

void AppMain::apActive()
{
}

void AppMain::apStopped()
{
}

void AppMain::staActive()
{
}

void AppMain::staStopped()
{
}
