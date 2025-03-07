#include "WiFi.h"
#include <string>
#include <vector>
#include "esp_http_server.h"

extern WiFi wifi;

class AppMain
{
public:
    AppMain() = default;

    void run();
    inline std::string hostname() { return m_hostname; };
    [[nodiscard]] inline bool stationConnected() const { return m_stationConnected; };

private:
    void processCommand(std::vector<std::string> args);
    esp_err_t mountStorage(const char *basePath);
    esp_err_t unmountStorage();

    std::string m_hostname;
    bool m_stationConnected;
    httpd_handle_t m_webserver;
    std::string m_otaServer;
    std::string m_ssid;
    std::string m_password;
    bool m_apMode = false;
    bool m_testConnection = false;

    std::string generateHostname(const std::string &hostname_base);

    void enterApMode();

    void apStarted();

    void apStopped();

    void tryConnectWifi(std::string ssid, std::string password);

    void spiffsUpdate(const char *from);

    void firmwareUpdate(const char *from);
};

extern AppMain appMain;
