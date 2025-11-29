#pragma once
#include <string>

namespace LiteAPI{
    class Shader;
    class ShaderConstructor;
    namespace ShaderAssets{
        Shader* loadFromConstructor(const ShaderConstructor &_constructor,std::string _name);
        Shader* get(std::string _name);
        void deleteOne(std::string _name);
        void deleteAll();
    }
}