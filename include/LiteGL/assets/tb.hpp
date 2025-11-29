#pragma once
#include <string>

namespace LiteAPI{
    class Texture;

    namespace TextureAssets{
        Texture* loadFromRes(std::string _path, std::string _name);
        Texture* get(std::string _name);
        void deleteOne(std::string _name);
        void deleteAll();
    }
}
