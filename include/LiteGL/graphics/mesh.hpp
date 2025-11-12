#pragma once
using uint64 = unsigned long long; 
using uint32 = unsigned int;

namespace LiteAPI{
    enum class Primitive{
        Points = 0x0,
        Lines = 0x1,
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
        ~Mesh();
        void reload(const float* _buffer,uint64 _vertices);
        void draw(Primitive _primitive);
        void draw_part(Primitive _primitive,uint64 _vertices,uint64 _offset = 0);
    };

    class DynamicMesh{
        private:
        uint32 vbo;
        uint32 vao;
        uint64 vertices;
        uint64 vertex_size;
        public:
        DynamicMesh(const float* _buffer,uint64 _vertices,const int* _attr);
        ~DynamicMesh();
        void reload(const float* _buffer,uint64 _vertices);
        void reload_part(const float* _buffer,uint64 _vertices,uint64 _offset = 0);
        void draw(Primitive _primitive);
        void draw_part(Primitive _primitive,uint64 _vertices,uint64 _offset = 0);
    };
}