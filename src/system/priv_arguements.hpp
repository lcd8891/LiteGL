#pragma once
#include <LiteGL/system/arguments.hpp>

namespace PRIV{
    namespace Args{
        #ifdef _WIN32
        void hold_console();
        #endif
        void parse_all(int argc,char** argv);
        void process_flags();
    }
}