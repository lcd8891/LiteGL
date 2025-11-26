#pragma once
using uint64 = unsigned long long; 
using uint32 = unsigned int;

namespace LiteAPI{
    class VertexArray;
    enum class Primitive{
        Points = 0x0,
        Lines = 0x1,
        Line_loop = 0x2,
        Line_strip = 0x3,
        Lines_adjacency = 0xa,
        Triangles = 0x4,
        Triangle_strip = 0x5
    };

    class Mesh{
        private:
        uint32 vbo;
        uint32 vao;
        uint64 vertices;
        uint64 vertex_size;
        public:
        Mesh(const float* _buffer,uint64 _vertices,const int* _attr);
        Mesh(VertexArray* array, const int* _attr);
        ~Mesh();
        void reload(const float* _buffer,uint64 _vertices);
        void reload(VertexArray* array);
        void draw(Primitive _primitive);
        void drawPart(Primitive _primitive,uint64 _vertices,uint64 _offset = 0);
    };

    class DynamicMesh{
        private:
        uint32 vbo;
        uint32 vao;
        uint64 vertices;
        uint64 vertex_size;
        public:
        DynamicMesh(const float* _buffer,uint64 _vertices,const int* _attr);
        DynamicMesh(VertexArray* array, const int* _attr);
        ~DynamicMesh();
        void reload(const float* _buffer,uint64 _vertices);
        void reload(VertexArray* array);
        void reloadPart(const float* _buffer,uint64 _vertices,uint64 _offset = 0);
        void reloadPart(VertexArray* array,uint64 _offset);
        void draw(Primitive _primitive);
        void drawPart(Primitive _primitive,uint64 _vertices,uint64 _offset = 0);
    };
}