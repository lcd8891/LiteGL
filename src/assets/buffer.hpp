#pragma once
#include <string>

namespace LiteAPI{
    class Texture;
}

namespace PRIV{
    void texture_buffer_set_mem(LiteAPI::Texture *obj,std::string _name);
}