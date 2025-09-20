#pragma once 

struct color;

namespace LiteAPI{
    namespace GLCtrl{
        void clear_color(color _color);
        void clear(bool color_buffer = true,bool depth_buffer = false);
    }
}