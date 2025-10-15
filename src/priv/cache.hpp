#pragma once
#include <filesystem>
#include <fstream>
#include <LiteGL/logger.hpp>
#include <LiteGL/graphics/shader.hpp>
#include <GL/glew.h>
#include <set>
#include <filesystem>

#define TO_CHAR(A) reinterpret_cast<char*>(&A)

const std::set<std::string> cache_folders = {
    "engine",
    "font"
};

namespace{
    bool check_folder(const std::filesystem::path &_path){
        return std::filesystem::exists(_path);
    }
}

namespace Cache{
    void create_cache_folder(){
        for(auto &str : cache_folders){
            std::filesystem::create_directories("./.lite-cache/"+str);
        }
    }
    void check_cache_folder(){
        if(std::filesystem::is_directory("./.lite-cache/")){
            for(auto& str : cache_folders){
                if(!std::filesystem::is_directory("./.lite-cache/"+str)){create_cache_folder();}
            }
        }else{
            create_cache_folder();
        }
    }
    void save_shader(std::string _path,LiteAPI::Shader* _shader){
        if(!check_folder(".lite-cache/engine/")){
            return;
        }
        unsigned int id = _shader->getID();
        int size;
        glGetProgramiv(id,GL_PROGRAM_BINARY_LENGTH,&size);
        char* data = new char[size];
        GLenum format = 0;
        glGetProgramBinary(id,size,nullptr,&format,data);
        std::string path = ".lite-cache/engine/"+_path+".bin";
        std::ofstream file(path,std::ios::binary);
        if(!file.is_open()){
            LiteAPI::Logger::error("Couldn't open cached shader: "+_path);
            return;
        }
        file.write(TO_CHAR(size),sizeof(int));
        file.write(data,size);
        file.write(TO_CHAR(format),sizeof(GLenum));
        file.close();
    }
    LiteAPI::Shader* load_shader(std::string _path){
        std::ifstream file(".lite-cache/engine/"+_path+".bin",std::ios::binary);
        if(!file.is_open()){
            LiteAPI::Logger::error("Couldn't open cached shader: "+_path);
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
            LiteAPI::Logger::warning("Couldn't load cached shader: "+_path);
            glDeleteProgram(id);
            return nullptr;
        }
        return new LiteAPI::Shader(id);
    }
}