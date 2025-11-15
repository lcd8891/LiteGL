#pragma once
#include <vector>

namespace LiteAPI{
    class VertexArray{
        private:
        unsigned vertex_size{0};
        std::vector<float> arr;

        public:
        class VertexIterator{
            private:
            float* current_ptr;
            unsigned int step;

            public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = float*;
            using difference_type = std::ptrdiff_t;
            using pointer = float**;
            using reference = float*&;
            VertexIterator(float* ptr, unsigned int vertex_size);

            VertexIterator operator++();
            VertexIterator operator++(int);
            VertexIterator& operator--() ;
            VertexIterator operator--(int) ;
            VertexIterator operator+(difference_type n) const ;
            VertexIterator operator-(difference_type n) const ;
            difference_type operator-(const VertexIterator& other) const ;
            bool operator==(const VertexIterator& other) const ;
            bool operator!=(const VertexIterator& other) const ;
            bool operator<(const VertexIterator& other) const ;
            bool operator>(const VertexIterator& other) const ;
            bool operator<=(const VertexIterator& other) const ;
            bool operator>=(const VertexIterator& other) const ;
            float* operator*() const ;
            float* operator->() const ;
            float* operator[](difference_type n) const ;
            VertexIterator& operator+=(difference_type n);
            VertexIterator& operator-=(difference_type n);
        };
        VertexArray(unsigned vertex_size = 1);
        unsigned getVertexSize();
        void insert(float* _data, unsigned vertex_count);
        void insert(float* _data, unsigned vertex_count,unsigned vertex_offset);
        void replace(float value,unsigned attribute,unsigned from_vertex,unsigned vertex_count = 1);
        void replace(float* value,unsigned from_vertex,unsigned size = 1);
        void erase(unsigned vertices,unsigned offset);
        float* getData();
        void reserver(unsigned _vertices);
        void clear();
        unsigned getVertexCount();
        VertexIterator begin();
        VertexIterator end();
        VertexIterator operator[](unsigned vertex);
    };
}