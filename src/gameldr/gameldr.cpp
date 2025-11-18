#include "gameldr.hpp"
#include <filesystem>
#include "../system/priv_logger.hpp"
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#define LIB_FORMAT ".dll"
HMODULE handle = nullptr;
#elif __linux__
#include <dlfcn.h>
#define LIB_FORMAT ".so"
void *handle = nullptr;
#endif


namespace LiteGame{
    std::string game_name;
    void (*on_initialize)();
    void (*on_frame)();
    void (*on_exit)();
}

std::filesystem::path path;

namespace GameLDR{
    std::string loadpath = "./litegl-game";
    void loadgame(){
        path = loadpath+LIB_FORMAT;
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
        #elif __linux__
            handle = dlopen(path.c_str(),RTLD_NOW | RTLD_GLOBAL);
            if(!handle){
                throw std::runtime_error("couldn't load game: "+path.string()+"\nReason: "+dlerror());
            }
            LiteGame::on_initialize = (void(*)())dlsym(handle,"game_on_initialize");
            LiteGame::on_frame = (void(*)())dlsym(handle,"game_on_frame");
            LiteGame::on_exit = (void(*)())dlsym(handle,"game_on_exit");
            LiteGame::game_name = *(const char**)dlsym(handle,"game_name");
        #endif
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
            throw std::runtime_error("game ptr load fail:\n"+ss.str());
        }
        if(LiteGame::game_name.empty()){
           system_logger->warn() << "Game name not found!";
           LiteGame::game_name = "litegl-game";
        }
        system_logger->info()<<"Loaded game: "+LiteGame::game_name;
    }
    void close(){
        if(!handle)return;
        #ifndef _WIN32
        dlclose(handle);
        #endif
    }
    void reload_game(){

    }

}