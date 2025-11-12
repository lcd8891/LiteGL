#pragma once

#include <string>

namespace LiteGame{
    extern std::string game_name;
    extern void (*on_initialize)();
    extern void (*on_frame)();
    extern void (*on_exit)();
}

namespace GameLDR{
    extern std::string loadpath;
    void loadgame();
    void close();
    void reload_game();
}