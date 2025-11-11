#include "priv_arguements.hpp"
#include <vector>
#include <functional>
#include <unordered_map>
#include <LiteGL/window/window.hpp>
#include "../gameldr/gameldr.hpp"

#ifdef _WIN32
#include <windows.h>
void enable_ascii(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
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
    void register_args(){
        #ifdef _WIN32
        arg_handler[Argument("output",'o')] = [](const std::string &_flag){
            enable_ascii();
        };
        #endif
        arg_handler[Argument("fullscreen",'f')] = [](const std::string &_flag){
            LiteAPI::Window::setFullscreen(true);
        };
        arg_handler[Argument("game=",0)] = [](const std::string &_flag){
            GameLDR::loadpath = LiteAPI::Arguments::getValueFromFlag(_flag);
        };
    }
    bool is_argument(const std::string &_arg){
        return _arg[0]=='-';
    }
    bool is_long_type(const std::string &_arg){
        return _arg[1]=='-';
    }
    void process_argument(const std::string &_arg){
        if(is_long_type(_arg)){
            std::string long_arg = _arg.substr(2);
            if(long_arg.find('=') != std::string::npos){
                auto it = arg_handler.find(Argument(long_arg.substr(0, long_arg.find('=')),0));
                if(it != arg_handler.end()) it->second(_arg);
            } else {
                auto it = arg_handler.find(Argument(long_arg,0));
                if(it != arg_handler.end()) it->second(_arg);
            }
        } else {
            for(size_t i = 1; i < _arg.length(); i++){
                char c = _arg[i];
                Argument search_arg("", c);
                auto it = arg_handler.find(search_arg);
                if(it != arg_handler.end()) it->second(_arg);
            }
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
        std::string getValueFromFlag(const std::string &arg,char divider){
            size_t pos = arg.find_first_of(divider);
            if(pos==std::string::npos){
                return "";
            }else{
                return arg.substr(pos+1);
            }
        }
    }
}
namespace PRIV{
    namespace Args{
        void process_flags(){
            for(std::string &_flag : args){
                auto it = flag_handler.find(_flag);
                if(it!=flag_handler.end())it->second(_flag);
            }
        }
        void parse_all(int argc,char** argv){
            register_args();
            for(unsigned i = 1;i<argc;i++){
                std::string arg = argv[i];
                args.push_back(arg);
                if(is_argument(arg)){
                    process_argument(arg);
                }
            }
            arg_handler.clear();
        }
    }
}