#pragma once
#include <string>
#include "types.hpp"

namespace LiteAPI{
    struct LiteDefaults{
        std::string window_title;
        vector2<uint16> window_size;
        std::string icon_path;
    };
    void setDefault(const LiteDefaults& _default);
}