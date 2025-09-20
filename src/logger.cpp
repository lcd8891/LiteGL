#include <LiteGL/logger.hpp>
#include <sstream>
#include <iomanip>
#include <LiteGL/time.hpp>
#include <iostream>

namespace LiteAPI{
    namespace Logger{
        void info(std::string _str){
            Time t = getCurrentTime();
            std::stringstream ss;
            ss << std::setfill('0');
            ss << "[" << std::setw(2) << +t.hour;
            ss << ":" << std::setw(2) << +t.minute;
            ss << ":" << std::setw(2) << +t.second << "]";
            if(LOGGER_GROUP!=""){
                ss << " (" << LOGGER_GROUP << ")";
            }
            ss << " INFO: " << _str;
            std::cout << ss.str() << '\n';
        }
        void error(std::string _str){
            Time t = getCurrentTime();
            std::stringstream ss;
            ss << std::setfill('0');
            ss << "[" << std::setw(2) << +t.hour;
            ss << ":" << std::setw(2) << +t.minute;
            ss << ":" << std::setw(2) << +t.second << "]";
            if(LOGGER_GROUP!=""){
                ss << " (" << LOGGER_GROUP << ")";
            }
            ss << " \e[0;31mERROR\e[0m: " << _str;
            std::cerr << ss.str() << '\n';
        }
        void warning(std::string _str){
            Time t = getCurrentTime();
            std::stringstream ss;
            ss << std::setfill('0');
            ss << "[" << std::setw(2) << +t.hour;
            ss << ":" << std::setw(2) << +t.minute;
            ss << ":" << std::setw(2) << +t.second << "]";
            if(LOGGER_GROUP!=""){
                ss << " (" << LOGGER_GROUP << ")";
            }
            ss << " \e[0;33mWARNING\e[0m: " << _str;
            std::cout << ss.str() << '\n';
        }
    }
}