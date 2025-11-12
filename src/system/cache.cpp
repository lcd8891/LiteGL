#include "priv_cache.hpp"
#include <LiteGL/graphics/shader.hpp>
#include "../system/priv_logger.hpp"
#include <GL/glew.h>
#include <fstream>
#include <filesystem>

#define TO_CHAR(A) reinterpret_cast<char*>(&A)
namespace {
    std::string genHash(std::string &in){
        return std::to_string(std::hash<std::string>{}(in));
    }
}
namespace Cache{
    void cleanup(){
        
    }
    void check_directories(){
        
    }
    void cache_font(){

    }
    void load_font(){
        
    }
    
    void cache_shaders(){
        
    }
    void load_shaders(){

    }

    void cache_shader(LiteAPI::Shader *shader,std::string name){
        unsigned id = shader->getID();
        int size;
        glGetProgramiv(id,GL_PROGRAM_BINARY_LENGTH,&size);
        char* data = new char[size];
        GLenum format = 0;
        glGetProgramBinary(id,size,nullptr,&format,data);
        std::ofstream file("./.cache/shaders"+genHash(name)+".bin",std::ios::binary);
        if(!file.is_open()){
            system_logger->error() << "Coudn't save shader " << name;
        }
        file.write(TO_CHAR(size),sizeof(size_t));
        file.write(data,size);
        file.write(TO_CHAR(format),sizeof(GLenum));
        file.close();
    }
    LiteAPI::Shader* load_chached_shader(std::string name){
        std::ifstream file(".lite-cache/engine/"+genHash(name)+".bin",std::ios::binary);
        if(!file.is_open()){
            system_logger->error() << "Couldn't open cached shader: " << name;
            return nullptr;
        }
        char *data;GLenum format;int size;
        file.read(TO_CHAR(size),sizeof(int));
        data = new char[size];
        file.read(data,size);
        file.read(TO_CHAR(format),sizeof(GLenum));
        file.close();
        unsigned int id = glCreateProgram();
        glProgramBinary(id,format,data,size);
        int status = 0;
        glGetProgramiv(id,GL_LINK_STATUS,&status);
        if(status != 1){
            system_logger->info() << "Couldn't load cached shader: " << name;
            glDeleteProgram(id);
            return nullptr;
        }
        return new LiteAPI::Shader(id);
    }
}