//
// Created by Melanie on 23/02/2025.
//

#ifndef LIGHTSWITCH_CONSOLE_H
#define LIGHTSWITCH_CONSOLE_H

extern "C" {
#include "console.h"
}
#include <string>
#include <vector>
#include <functional>

class Console
{
public:
    enum ConsoleType
    {
        TelnetConsole,
        UartConsole
    };
    Console(ConsoleType type, std::function<void(std::vector<std::string>)>, std::function<void (void)> onConnect = {});

private:
    void connectHandler();

    static void processCommandThunk(int argc, char **argv, void *user_data);
    static void connectHandlerThunk(void *user_data);

    ConsoleType m_type;
    struct console_handlers c_handlers = {};

//    void (*m_process)(std::vector<std::string>) = nullptr;
    std::function<void(std::vector<std::string>)> m_process;
    std::function<void (void)> m_onConnect = [this] { connectHandler(); };
};


#endif //LIGHTSWITCH_CONSOLE_H
