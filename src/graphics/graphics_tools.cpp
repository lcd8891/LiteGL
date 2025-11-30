#include "graphics_tools.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <GL/glew.h>

LiteAPI::Texture* _load_texture_from_file(const std::string _path){
    unsigned int textureID;
    glGenTextures(1,&textureID);
    int width,heigth,component;
    
    unsigned char* imagedata = stbi_load(_path.c_str(),&width,&heigth,&component,0);
    if(imagedata){
        int format;
        switch(component){
            case 1:
            format = GL_RED;
            break;
            case 3:
            format = GL_RGB;
            break;
            case 4:
            format = GL_RGBA;
        }
        glBindTexture(GL_TEXTURE_2D,textureID);
        glTexImage2D(GL_TEXTURE_2D,0,format,width,heigth,0,format,GL_UNSIGNED_BYTE,imagedata);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        stbi_image_free(imagedata);
        return new LiteAPI::Texture(textureID);
    }else{
        throw std::runtime_error("Couldn't load texture: "+_path);
    }
}
        