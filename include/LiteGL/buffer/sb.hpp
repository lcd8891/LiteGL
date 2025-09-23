#pragma once
#include <string>

namespace LiteAPI{
    class Shader;
    class ShaderConstructor;
    namespace ShaderBuffer{
        Shader* load_from_constructor(const ShaderConstructor &_constructor,std::string _name);
        Shader* get_shader(std::string _name);
        void delete_shader(std::string _name);
        void delete_all_shaders();
    }
}