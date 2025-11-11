#pragma once
#include <LiteGL/system/arguments.hpp>

namespace PRIV{
    namespace Args{
        void parse_all(int argc,char** argv);
        void process_flags();
    }
}