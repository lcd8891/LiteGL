#pragma once
#include <LiteGL/graphics/mesh.hpp>
#include <LiteGL/graphics/vertexarray.hpp>
using int8 = char;
template<typename T>
struct vector2;
struct color4;

namespace LiteAPI{
    class ScreenItem{
        protected:

        public:
        ScreenItem(vector2<int> a, color4 c);
        int8 order;
        vector2<int> position;
        color4 color;
        virtual VertexArray* getMesh() = 0;
        virtual bool isTextured() = 0;
        virtual Primitive getPrimitive() = 0;
    };
    class RectangleItem : public ScreenItem{
        public:
        vector2<unsigned> size;
        RectangleItem(vector2<int> a,vector2<unsigned> b,color4 c);

        VertexArray* getMesh() override; 
        bool isTextured() override {return false;};
        Primitive getPrimitive() override {return Primitive::Triangles;};
    };
    class LineItem : public ScreenItem{
        public:
        vector2<int> position2;
        LineItem(vector2<int> a,vector2<int> b,color4 c);

        VertexArray* getMesh() override; 
        bool isTextured() override {return false;};
        Primitive getPrimitive() override {return Primitive::Lines;};
    };
}