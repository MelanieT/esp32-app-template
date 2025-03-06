#include "WiFi.h"
#include "string"

extern WiFi wifi;

class AppMain
{
public:
    AppMain() = default;

    void run();
    inline std::string hostname() { return m_hostname; };
    inline bool stationConnected() const { return m_stationConnected; };

private:
    std::string m_hostname;
    bool m_stationConnected;
};

extern AppMain appMain;
