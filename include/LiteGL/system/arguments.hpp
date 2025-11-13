#pragma once
#include <string>
#include <functional>

namespace LiteAPI{
    namespace Arguments{
        bool hasArgument(std::string _str);
        void registerFlag(const std::string &_arg,std::function<void()> _func);
    }
}