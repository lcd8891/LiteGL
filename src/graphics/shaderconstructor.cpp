#include <LiteGL/graphics/shaderconstructor.hpp>
#include <sstream>
#include <fstream>
#define LOGGER_GROUP "ShaderConstructor"
#include "../system/priv_logger.hpp"
#include <stdexcept>
#include <GL/glew.h>

unsigned int _compileshader(const std::string &_code,LiteAPI::ShaderType _type){
    unsigned int tmp;
    switch(_type){
        case LiteAPI::ShaderType::Vertex:
        tmp = glCreateShader(GL_VERTEX_SHADER);
        break;
        case LiteAPI::ShaderType::Geometry:
        tmp = glCreateShader(GL_GEOMETRY_SHADER);
        break;
        case LiteAPI::ShaderType::Fragment:
        tmp = glCreateShader(GL_FRAGMENT_SHADER); 
        break;
    }
    const char* code = _code.c_str();
    glShaderSource(tmp,1,&code,nullptr);
    glCompileShader(tmp);
    std::string info;
    int status;
    glGetShaderiv(tmp,GL_COMPILE_STATUS,&status);
    if(!status){
        char log[512];
        glGetShaderInfoLog(tmp,512,nullptr,&log[0]);
        throw std::runtime_error("Compile error: "+std::string(log));
    }
    return tmp;
}

namespace LiteAPI{
    ShaderConstructor::_ShaderProgram::_ShaderProgram(unsigned int _id, LiteAPI::ShaderType _type):ID(_id),type(_type){}
    ShaderConstructor::_ShaderProgram::~_ShaderProgram(){
        glDeleteShader(this->ID);
    }
    ShaderConstructor::ShaderConstructor():size(0){
        programs = new _ShaderProgram*[128];
    }
    ShaderConstructor::~ShaderConstructor(){
        this->clear();
        delete[] this->programs;
    }
    bool ShaderConstructor::addFromFile(std::string _path,ShaderType _type){
        std::ifstream file("./res/shaders/"+_path);
        if(!file.is_open()){system_logger->error() << ("Couldn't open file: ./res/shaders/"+_path);}
        std::stringstream ss;
        ss << file.rdbuf();
        unsigned int shader;
        try{
            shader = _compileshader(ss.str(),_type);
        }catch(const std::exception &e){
            system_logger->error() << (std::string(e.what()));
            return false;
        }
        this->paste(new _ShaderProgram(shader,_type));
        return true;
    }
    bool ShaderConstructor::addFromString(std::string _code,ShaderType _type){
        unsigned int shader;
        try{
            shader = _compileshader(_code,_type);
        }catch(const std::exception &e){
            system_logger->error() << (std::string(e.what()));
            return false;
        }
        this->paste(new _ShaderProgram(shader,_type));
        return true;
    }
    void ShaderConstructor::clear(){
        for(int i = 0;i<this->size;i++){
            delete this->programs[i];
        }
        this->size = 0;
    }
    Shader* ShaderConstructor::create() const {
        unsigned int shader = glCreateProgram();
        for(int i = 0;i<this->size;i++){
            glAttachShader(shader,this->programs[i]->ID);
        }
        glLinkProgram(shader);int status;
        glGetProgramiv(shader,GL_LINK_STATUS,&status);
        if(!status){
            char log[512];
            glGetProgramInfoLog(shader,512,nullptr,&log[0]);
            system_logger->error() << ("Linking error: "+std::string(log));
            glDeleteProgram(shader);
            return nullptr;
        }
        return new Shader(shader);
    }
    void ShaderConstructor::paste(_ShaderProgram *_prog){
        if(size==128)throw std::runtime_error("ShaderConstructor overcreate\n(shaders count in 1 shaderconstructor limit reached)");
        this->programs[size] = _prog;
        size++;
    }
}