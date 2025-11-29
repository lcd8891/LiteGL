#pragma once
#include <string>

namespace LiteAPI{
    class Screen;
    namespace ScreenAssets{
        Screen* create(std::string _name);
        Screen* get(std::string _name);
        void deleteOne(std::string _name);
        void deleteAll();
    }
}