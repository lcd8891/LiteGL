#include "priv_arguements.hpp"
#include <vector>
#include <functional>
#include <unordered_map>
#include <LiteGL/window/window.hpp>
#include "../gameldr/gameldr.hpp"
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <iostream>
#include <limits>
void enable_ascii(){
    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONIN$", "r", stdin);
    freopen_s(&dummy, "CONOUT$", "w", stderr);
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    
    SetConsoleTitleA("LiteGL Output Console");

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hOut, &consoleInfo);
    COORD bufferSize;
    bufferSize.X = consoleInfo.dwSize.X;
    bufferSize.Y = 500;
    SetConsoleScreenBufferSize(hOut, bufferSize);
}
#endif

class Argument{
    public:
    std::string full_arg;
    char short_arg;
    Argument(std::string _full_arg,char _short_arg):full_arg(_full_arg),short_arg(_short_arg){}
    bool operator==(const Argument &v) const {
        return (short_arg == v.short_arg) || (full_arg == v.full_arg);
    }
    bool operator==(const char &v){
        return short_arg==v;
    }
    bool operator==(const std::string &v){
        return full_arg==v;
    }
};

namespace std {
    template<>
    struct hash<Argument> {
        std::size_t operator()(const Argument& arg) const {
            return std::hash<std::string>{}(arg.full_arg) ^ 
                   (std::hash<char>{}(arg.short_arg) << 1);
        }
    };
}

namespace{
    std::vector<std::string> args;
    std::unordered_map<std::string,std::function<void(const std::string &_flag)>> flag_handler;
    std::unordered_map<Argument,std::function<void(const std::string &_flag)>> arg_handler;
    void register_args(){ // Регистрация системных аргументов
        #ifdef _WIN32
        arg_handler[Argument("output",'o')] = [](const std::string &_value){
            enable_ascii();
        };
        #endif
        arg_handler[Argument("fullscreen",'f')] = [](const std::string &_value){
            LiteAPI::Window::setFullscreen(true);
        };
        arg_handler[Argument("game=",0)] = [](const std::string &_value){
            GameLDR::loadpath="./"+_value;
        };
        arg_handler[Argument("reload",'r')] = [](const std::string &_value){
            std::filesystem::remove(".cache");
        };
    }
    bool is_argument(const std::string &_arg){ // Проверка на аргумент с -
        return _arg[0]=='-';
    }
    bool is_long_type(const std::string &_arg){ // Является ли аргумент длинным --arg
        return _arg[1]=='-';
    }
    std::string extract_long_arg_name(const std::string& long_arg) {
        size_t equal_pos = long_arg.find('=');
        if (equal_pos != std::string::npos) {
            return long_arg.substr(0, equal_pos);
        }
        return long_arg;
    }
    std::pair<std::string, std::string> parse_long_argument(const std::string& long_arg) {
        std::string arg_name = long_arg.substr(2);
        std::string value;
        
        size_t equal_pos = arg_name.find('=');
        if (equal_pos != std::string::npos) {
            value = arg_name.substr(equal_pos + 1);
            arg_name = arg_name.substr(0, equal_pos);
        }
        
        return {arg_name, value};
    }
    void process_long_argument(const std::string& _arg) {
        auto [arg_name, value] = parse_long_argument(_arg);
        auto it = arg_handler.find(Argument(arg_name, 0));
        if (it != arg_handler.end()) {
            it->second(value);
            return;
        }
        for (const auto& handler : arg_handler) {
            if (handler.first.full_arg.find(arg_name) == 0) {
                handler.second(value);
                return;
            }
        }
    }
    void process_short_argument(const std::string& _arg) {
        for(size_t i = 1; i < _arg.length(); i++) {
            char c = _arg[i];
            bool found = false;
            for(const auto& handler : arg_handler) {
                if(handler.first.short_arg == c) {
                    handler.second(_arg);
                    found = true;
                    break;
                }
            }
        }
    }
    void process_argument(const std::string &_arg){ // Выполнение аргумента
        if(is_long_type(_arg)) {
            process_long_argument(_arg);
        } else {
            process_short_argument(_arg);
        }
    }
}
namespace LiteAPI{
    namespace Arguments{
        bool hasArgument(std::string _str){
            for(std::string &str : args){
                if(str==_str)return true;
            }
            return false;
        }
        void registerFlag(const std::string &_arg,std::function<void(const std::string &_flag)> _func){
            auto it = flag_handler.find(_arg);
            if(it==flag_handler.end()){
                flag_handler[_arg]=_func;
            }else{
                it->second=_func;
            }
        }
    }
}
namespace PRIV{
    namespace Args{
        #ifdef _WIN32
        void hold_console(){ // Удержание консоли
            if(GetConsoleWindow()){
                std::cout << "\n\nPress Enter to exit..." << std::endl;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
            }
        }
        #endif
        void process_flags(){ //Выполнение флагов
            for(std::string &_flag : args){
                auto it = flag_handler.find(_flag);
                if(it!=flag_handler.end())it->second(_flag);
            }
        }
        void parse_all(int argc,char** argv){ //Проверка аргументов
            register_args();
            args.clear();
            
            for(int i = 1; i < argc; i++){
                std::string arg = argv[i];
                args.push_back(arg);
                if(is_argument(arg)){
                    process_argument(arg);
                }
            }
        }
    }
}