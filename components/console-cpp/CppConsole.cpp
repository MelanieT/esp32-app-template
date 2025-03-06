//
// Created by Melanie on 23/02/2025.
//

#include "CppConsole.h"

#include <utility>
#include <cstring>

extern "C" {
#include "uart-console.h"
#include "telnetd-bsd.h"
}

using namespace std;

Console::Console(Console::ConsoleType type, std::function<void(std::vector<std::string>)> processor,  function<void (void)> onConnect)
{
    m_type = type;
    m_process = processor;
    if (onConnect)
        m_onConnect = std::move(onConnect);

    if (type == TelnetConsole)
    {
        telnetd_init(23);

        c_handlers.send = telnetd_send;
        c_handlers.disconnect = telnetd_disconnect;
        c_handlers.set_process_line_cb = telnetd_set_process_line_cb;
        c_handlers.set_interrupt_cb = telnetd_set_interrupt_cb;
        c_handlers.set_new_connection_cb = telnetd_set_new_connection_cb;
        c_handlers.process_command_cb = &processCommandThunk;
        c_handlers.new_connection_cb = &connectHandlerThunk;
        c_handlers.user_data = this;
        console_set_handlers(&c_handlers);
    }
    else
    {
        uart_init();
        c_handlers.send = uart_send;
        c_handlers.disconnect = uart_disconnect;
        c_handlers.set_process_line_cb = uart_set_process_line_cb;
        c_handlers.set_interrupt_cb = uart_set_interrupt_cb;
        c_handlers.set_new_connection_cb = uart_set_new_connection_cb;
        c_handlers.process_command_cb = &processCommandThunk;
        c_handlers.new_connection_cb = &connectHandlerThunk;
        c_handlers.user_data = this;
        console_set_handlers(&c_handlers);
    }

    console_init();
}

void Console::connectHandler()
{

}

void Console::connectHandlerThunk(void *user_data)
{
    ((Console *)user_data)-> connectHandler();
}

void Console::processCommandThunk(int argc, char **argv, void *user_data)
{
    // Force copy because the argv[] array will be freed as soon as this returns
    vector<string> args;
    for (int i = 0 ; i < argc ; i++)
    {
        string s;
        s.resize(strlen(argv[i]));
        strcpy(s.data(), argv[i]);
        args.push_back(s);
    }

    if (args.size() == 1 && args[0] == "quit")
    {
        console_disconnect();
        return;
    }

    if (user_data)
        ((Console *)user_data)->m_process(args);
}
