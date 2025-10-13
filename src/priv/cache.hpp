#pragma once
#include <filesystem>
#include <fstream>
#include <LiteGL/logger.hpp>
#include <LiteGL/graphics/shader.hpp>
#include <GL/glew.h>

#define TO_CHAR(A) reinterpret_cast<char*>(&A)

namespace Cache{
    void create_cache_folder(){
        std::filesystem::create_directories("./.lite-chache/engine");
    }
    void check_cache_folder(){
        if(std::filesystem::is_directory("./.lite-chache/")){
            
        }else{
            create_cache_folder();
        }
    }
    void save_shader(std::string _path,LiteAPI::Shader* _shader){
        unsigned int id = _shader->getID();
        int size;
        glGetProgramiv(id,GL_PROGRAM_BINARY_LENGTH,&size);
        char* data = new char[size];
        GLenum format = 0;
        glGetProgramBinary(id,size,nullptr,&format,data);
        std::ofstream file("./.lite-chache/engine/"+_path,std::ios::binary);
        file.write(TO_CHAR(size),sizeof(int));
        file.write(data,size);
        file.write(TO_CHAR(format),sizeof(GLenum));
        file.close();
    }
    LiteAPI::Shader* load_shader(std::string _path){
        std::ifstream file("./.lite-chache/engine/"+_path,std::ios::binary);
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
            LiteAPI::Logger::warning("Couldn't load cached shader: "+_path);
            glDeleteProgram(id);
            return nullptr;
        }
        return new LiteAPI::Shader(id);
    }
}