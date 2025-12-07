#pragma once
#include <LiteGL/graphics/mesh.hpp>
#include <LiteGL/graphics/vertexarray.hpp>
using int8 = char;
template<typename T>
struct vector2;
struct color4;

namespace LiteAPI{
    class Font;
    enum class ScreenItemType{
        Rectangle,Line,Texture,Text
    };
    enum class ScreenItemState{
        Created,Modified,Idle
    };
    class ScreenItem{
        protected:
        vector2<int> position;
        color4 color;
        vector2<float> relative;
        ScreenItemState state = ScreenItemState::Created;

        public:
        ScreenItem(vector2<int> a, color4 c,vector2<float> r);

        virtual VertexArray* getMesh() = 0;
        virtual ScreenItemType getType() = 0;
        virtual Primitive getPrimitive() = 0;

        vector2<int> getPosition();
        color4 getColor();
        ScreenItemState& getItemState();
        vector2<float> getRelative();
        bool relativeIsZero();
        template<typename T>
        T* toItemType(){
            return static_cast<T*>(this);
        }

        void setColor(color4 _a);
        void setPosition(vector2<int> _a);
        void setRelative(vector2<float> _r);
    };
    class RectangleItem : public ScreenItem{
        private:
        vector2<unsigned> size;
        public:
        RectangleItem(vector2<int> a,vector2<unsigned> b,color4 c,vector2<float> r = {0,0});

        VertexArray* getMesh() override; 
        ScreenItemType getType() override {return ScreenItemType::Rectangle;};
        Primitive getPrimitive() override {return Primitive::Triangles;};

        vector2<unsigned> getSize();
        void setSize(vector2<unsigned> a);
    };
    class LineItem : public ScreenItem{
        private:
        vector2<int> position2;
        public:
        LineItem(vector2<int> a,vector2<int> b,color4 c,vector2<float> r = {0,0});

        VertexArray* getMesh() override; 
        ScreenItemType getType() override {return ScreenItemType::Line;};
        Primitive getPrimitive() override {return Primitive::Lines;};

        vector2<int> getPosition2();
        void setPosition2(vector2<int> a);
    };
    class TextureItem : public ScreenItem{
        float *uv;
        vector2<unsigned> size;
        std::string texture_key;
        public:
        TextureItem(vector2<int> a,color4 c,vector2<unsigned> b,std::string _tex_key,vector2<float> uv1,vector2<float> uv2,vector2<float> r = {0,0});
        ~TextureItem();

        VertexArray* getMesh() override;
        ScreenItemType getType() override {return ScreenItemType::Texture;};
        Primitive getPrimitive() override {return Primitive::Triangles;}

        const float* getUV();
        void setUV(float u1,float v1,float u2,float v2);
        vector2<unsigned> getSize();
        void setSize(vector2<unsigned> a);
        std::string getTextureKey();
        void setTextureKey(std::string _key);
    };
    class TextItem : public ScreenItem{
        std::wstring str;
        float scale;
        Font* font;
        public:
        TextItem(vector2<int> a,color4 b,std::wstring c,std::string f,float s = 1.f,vector2<float> r = {0,0});
        
        VertexArray* getMesh() override;
        ScreenItemType getType() override {return ScreenItemType::Text;};
        Primitive getPrimitive() override {return Primitive::Triangles;}

        std::wstring getString();
        void setString(std::wstring str);
        float getTextScale();
        void setTextScale(float scale);
    };
}