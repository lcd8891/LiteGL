#pragma once
#include <string>

#ifndef LOGGER_GROUP
    #define LOGGER_GROUP ""
#endif

namespace LiteAPI{
    namespace Logger{
        void info(std::string _str);
        void error(std::string _str);
        void warning(std::string _str);
    }
}