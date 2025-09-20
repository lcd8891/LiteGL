#pragma once
#include <string>

template<typename T>
struct vector2;

namespace LiteAPI{
    class Texture{
        private:
        unsigned int id;
        public:
        Texture(unsigned int _ID);
        Texture(const std::string &_path);
        Texture(void *_buffer,size_t _buffer_size);
        ~Texture();

        void use();
    };
}