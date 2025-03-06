# console-cpp
A C++ wrapper for my console group of modules

Requires base_console, telnetd-bsd and uart_console.

To use, create a console object after your wifi is up:

console = new Console(Console::ConsoleType::TelnetConsole, processCommand);

The processCommand callback will be called with a singe vector<string> that contains the split arguments.
