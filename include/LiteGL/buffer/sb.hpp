#pragma once
#include <string>

namespace LiteAPI{
    class Shader;
    class ShaderConstructor;
    namespace ShaderBuffer{
        Shader* load_from_res(ShaderConstructor _constructor,std::string _name);
        Shader* get(std::string _name);
        void delete_shader(std::string _name);
        void delete_all();
    }
}