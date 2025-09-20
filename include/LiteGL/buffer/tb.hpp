#pragma once
#include <string>

namespace LiteAPI{
    class Texture;

    namespace TextureBuffer{
        Texture* load_from_res(std::string _path, std::string _name);
        Texture* get(std::string _name);
        void delete_texture(std::string _name);
        void delete_all_textures();
    }
}
