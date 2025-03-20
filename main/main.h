#ifndef APP_MAIN_H
#define APP_MAIN_H

#include "AppFramework.h"

class AppMain : AppFrameworkHandler
{
public:
    AppMain() = default;

    void run();

    void apActive() override;
    void apStopped() override;
    void staActive() override;
    void staStopped() override;
};

#endif