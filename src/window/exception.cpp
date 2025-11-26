#include "exception.hpp"
#include "iostream"
#include "../system/priv_logger.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#include <cstring>
#endif

void show_error(const std::string _str){
#ifdef _WIN32
    MessageBoxA(
        NULL, 
        _str.c_str(), 
        "Error", 
        MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
    );
#else
    bool dialog_shown = false;
        
    const char* dialog_commands[] = {
        "kdialog --error \"%s\" --title=\"LiteGL error\"",
        "zenity --error --text=\"%s\" --title=\"LiteGL error\"",
        "xmessage -center \"%s\"",
        "yad --error --text=\"%s\" --title=\"LiteGL error\"",
        "gxmessage \"%s\"",
        nullptr
    };
        
    for (int i = 0; dialog_commands[i] != nullptr; ++i) {
        char command[2048];
        snprintf(command, sizeof(command), dialog_commands[i], _str.c_str());
        
        int result = system(command);
        if (result == 0 || WEXITSTATUS(result) == 0) {
            dialog_shown = true;
            break;
        }
    }
    system_logger->error() << "Execution terminated!";
    if (!dialog_shown) {
        system_logger->error() << "Couldn't display graphical error dialog!";
        std::cerr << "===EXECUTION TERMINATED===\n" << _str << '\n';
    }
#endif
}