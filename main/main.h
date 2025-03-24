#ifndef APP_MAIN_H
#define APP_MAIN_H

#include "AppFramework.h"
#include <vector>
#include <string>

class AppMain : AppFrameworkHandler
{
public:
    AppMain() = default;

    void run();

    // These can be used if needed
    // void apActive() override;
    // void apStopped() override;
    void staActive() override;
    void staStopped() override;

private:
    void processCommand(std::vector<std::string> args);
};

#endif