#pragma once
using int8 = char;
template<typename T>
struct vector2;

namespace LiteAPI{
    class ScreenItem{
        protected:

        public:
        int8 order;
        vector2<int> position;
        virtual float* getMesh() = 0;
        const bool textured;
    };
    class RectangleItem : public ScreenItem{
        public:
        const bool textured = false;
    };
    class LineItem : public ScreenItem{
        public:
        const bool textured = true;
    };
}