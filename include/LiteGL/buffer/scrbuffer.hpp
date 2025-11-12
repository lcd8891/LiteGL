#pragma once
#include <string>

namespace LiteAPI{
    class Screen;
    namespace ScreenBuffer{
        Screen* create_screen(std::string _name);
        Screen* get_screen(std::string _name);
        void delete_screen(std::string _name);
        void delete_all_screens();
    }
}