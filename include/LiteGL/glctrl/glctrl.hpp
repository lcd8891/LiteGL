#pragma once 

struct color4;

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
        enum class BlendOption{
            Zero=0,
            One=1,
            OneMinusSrcAlpha=0x0303
        };
        void clear_color(color4 _color);
        void clear(bool color_buffer = true,bool depth_buffer = false);
        void setBlend(bool _enable);
        void blendFunc(BlendOption option);
    }
}