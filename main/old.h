#include "WiFi.h"
#include <string>
#include <vector>
#include "esp_http_server.h"
#include "AppFramework.h"

extern WiFi wifi;

class AppMain : AppFrameworkHandler
{
public:
    AppMain() = default;

    void run();
    inline std::string hostname() { return m_hostname; };
    void apStarted();
    void apStoppedOld();
    void enterApMode();
    void tryConnectWifi(std::string ssid, std::string password);
    [[nodiscard]] inline bool stationConnected() const { return m_stationConnected; };
    inline std::string ssid() { return m_ssid; };

    void apActive() override;
    void apStopped() override;
    void staActive() override;
    void staStopped() override;

private:
    void processCommand(std::vector<std::string> args);
    esp_err_t mountStorage(const char *basePath);
    esp_err_t unmountStorage();
    std::string generateHostname(const std::string &hostname_base);
    void spiffsUpdate(const char *from);
    void firmwareUpdate(const char *from);
    static void retryStationModeThunk(TimerHandle_t data);
    void retryStationMode(TimerHandle_t handle) const;

    std::string m_hostname;
    bool m_stationConnected;
    httpd_handle_t m_webserver;
    std::string m_otaServer;
    std::string m_ssid;
    std::string m_password;
    bool m_apMode = false;
    bool m_testConnection = false;
    TimerHandle_t m_stationModeRetryTimer = nullptr;
};

extern AppMain appMain;
