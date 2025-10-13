#pragma once
#include <glm/matrix.hpp>
#include <string>

namespace LiteAPI{
    class Shader{
        private:
        unsigned int id;
        public:
        Shader(unsigned int _id);
        Shader(std::string _path);
        ~Shader();

        void bind();
        void uniformMatrix(std::string name,glm::mat4 _mat);
        void uniformInt(std::string name,int _int);
        void uniformFloat(std::string name,float _float);
        void uniform2Float(std::string name,float _x,float _y);
        void uniform3Float(std::string name,float _x,float _y,float _z);
        unsigned int getID();
    };
}