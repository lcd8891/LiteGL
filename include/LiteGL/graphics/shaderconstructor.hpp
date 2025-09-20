#pragma once
#include "shader.hpp"

namespace LiteAPI{
    enum class ShaderType;
}

struct _ShaderProgram{
    unsigned int ID;
    LiteAPI::ShaderType type;  
};

namespace LiteAPI{
    enum class ShaderType{
        Vertex,
        Geometry,
        Fragment
    };

    class ShaderConstructor{
        private:
        _ShaderProgram** programs;
        void paste(const _ShaderProgram &_prog);

        public:
        ShaderConstructor();
        ~ShaderConstructor();
        bool addFromFile(std::string _path,ShaderType _type);
        bool addFromString(std::string _code,ShaderType _type);
        
        Shader* create() const;
        void clear();
    };
}