#pragma once
#include "shader.hpp"

using uint8 = unsigned char;

namespace LiteAPI{
    enum class ShaderType{
        Vertex,
        Geometry,
        Fragment
    };

    class ShaderConstructor{
        private:
        struct _ShaderProgram{
            unsigned int ID;
            LiteAPI::ShaderType type;  
            _ShaderProgram(unsigned int _id, LiteAPI::ShaderType _type);
            ~_ShaderProgram();
        };
        _ShaderProgram** programs;
        uint8 size;
        void paste(_ShaderProgram *_prog);

        public:
        ShaderConstructor();
        ~ShaderConstructor();
        bool addFromFile(std::string _path,ShaderType _type);
        bool addFromString(std::string _code,ShaderType _type);
        
        Shader* create() const;
        void clear();
    };
}