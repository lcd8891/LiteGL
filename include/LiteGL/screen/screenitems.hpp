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
        bool modified = true;
        vector2<int> position;
        color4 color;

        public:
        ScreenItem(vector2<int> a, color4 c);

        virtual VertexArray* getMesh() = 0;
        virtual bool isTextured() = 0;
        virtual Primitive getPrimitive() = 0;

        vector2<int> getPosition();
        color4 getColor();
        const bool& getModified();

        void setColor(color4 _a);
        void setPosition(vector2<int> _a);
    };
    class RectangleItem : public ScreenItem{
        private:
        vector2<unsigned> size;
        public:
        RectangleItem(vector2<int> a,vector2<unsigned> b,color4 c);

        VertexArray* getMesh() override; 
        bool isTextured() override {return false;};
        Primitive getPrimitive() override {return Primitive::Triangles;};

        vector2<unsigned> getSize();
        void setSize(vector2<unsigned> a);
    };
    class LineItem : public ScreenItem{
        private:
        vector2<int> position2;
        public:
        LineItem(vector2<int> a,vector2<int> b,color4 c);

        VertexArray* getMesh() override; 
        bool isTextured() override {return false;};
        Primitive getPrimitive() override {return Primitive::Lines;};

        vector2<int> getPosition2();
        void setPosition2(vector2<int> a);
    };
    class TextureItem : public ScreenItem{
        float *uv;
        vector2<unsigned> size;
        public:
        TextureItem(vector2<int> a,color4 c,vector2<unsigned> b,vector2<float> uv1,vector2<float> uv2);
        ~TextureItem();

        VertexArray* getMesh() override;
        bool isTextured() override {return true;}
        Primitive getPrimitive() override {return Primitive::Triangles;}

        const float* getUV();
        void setUV(float u1,float v1,float u2,float v2);
        vector2<unsigned> getSize();
        void setSize(vector2<unsigned> a);
    };
}