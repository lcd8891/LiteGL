#include <LiteGL/graphics/shader.hpp>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace LiteAPI{
    Shader::Shader(unsigned int _ID):id(_ID){}
    Shader::~Shader(){
        glDeleteProgram(id);
    }
    void Shader::bind(){
        glUseProgram(id);
    }
    void Shader::uniformMatrix(std::string name,glm::mat4 _mat){
        GLuint location = glGetUniformLocation(id,name.c_str());
        glUniformMatrix4fv(location,1,GL_FALSE,glm::value_ptr(_mat));
    }
    void Shader::uniformInt(std::string name,int _int){
        GLuint location = glGetUniformLocation(id,name.c_str());
        glUniform1i(location,_int);
    }
    void Shader::uniformFloat(std::string name,float _float){
        GLuint location = glGetUniformLocation(id,name.c_str());
        glUniform1f(location,_float);
    }
    void Shader::uniform2Float(std::string name,float _x,float _y){
        GLuint location = glGetUniformLocation(id,name.c_str());
        glUniform2f(location,_x,_y);
    }
    void Shader::uniform3Float(std::string name,float _x,float _y,float _z){
        GLuint location = glGetUniformLocation(id,name.c_str());
        glUniform3f(location,_x,_y,_z);
    }
    unsigned int Shader::getID(){
        return this->id;
    }
}