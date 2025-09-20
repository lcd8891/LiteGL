#include "gameldr.hpp"
#include <filesystem>
#include <LiteGL/logger.hpp>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#define LIB_FORMAT ".dll"
static HMODULE handle = nullptr;
#elif __linux__
#endif


namespace LiteGame{
    std::string game_name;
    void (*on_initialize)();
    void (*on_frame)();
    void (*on_exit)();
}

std::filesystem::path path;

namespace GameLDR{
    void loadgame(std::string _path){
        path = _path+LIB_FORMAT;
        if(!std::filesystem::exists(path) && !std::filesystem::is_directory(path)){
            throw std::runtime_error("game file not found: "+path.string());
        }
        #ifdef _WIN32
            handle = LoadLibraryA(path.string().c_str());
            if(!handle){
                throw std::runtime_error("couldn't load game: "+path.string());
            }
            LiteGame::on_initialize = (void(*)())GetProcAddress(handle,"game_on_initialize");
            LiteGame::on_frame = (void(*)())GetProcAddress(handle,"game_on_frame");
            LiteGame::on_exit = (void(*)())GetProcAddress(handle,"game_on_exit");
            LiteGame::game_name = *(const char**)GetProcAddress(handle,"game_name");
            std::stringstream ss;
            if(!LiteGame::on_initialize){
                ss << "Function game_on_initialize not found!\n";
            }
            if(!LiteGame::on_frame){
                ss << "Function game_on_frame not found!\n";
            }
            if(!LiteGame::on_exit){
                ss << "Function game_on_exit not found!\n";
            }
            if(!ss.str().empty()){
                throw std::runtime_error("gmae ptr load fail:\n"+ss.str());
            }
        #elif __linux__
        #endif
        if(LiteGame::game_name.empty()){
            LiteAPI::Logger::warning("Game name not found!");
        }
        LiteAPI::Logger::info("loaded game: "+LiteGame::game_name);
    }
    void reload_game(){

    }

}