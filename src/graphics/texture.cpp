#include <LiteGL/graphics/texture.hpp>
#include <LiteGL/types.hpp>
#include <stdexcept>
#include <GL/glew.h>

namespace LiteAPI{
    Texture::Texture(unsigned int _ID):id(_ID){}
    Texture::~Texture(){
        glDeleteTextures(1,&id);
    }

    void Texture::use(){
        glBindTexture(GL_TEXTURE_2D,id);
    }
}