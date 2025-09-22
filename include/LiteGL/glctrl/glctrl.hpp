#pragma once 

struct color;

namespace LiteAPI{
    namespace GLCtrl{
        enum class CullFace{
            Front = 0x0404,
            Back = 0x0405,
            FrontAndBack = 0x0408
        };
        enum class FrontFace{
            Clockwise = 0x0900,
            Counterclockwise = 0x0901
        };
        void clear_color(color _color);
        void clear(bool color_buffer = true,bool depth_buffer = false);
    }
}