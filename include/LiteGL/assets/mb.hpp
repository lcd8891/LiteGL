#pragma once
#include <string>

namespace LiteAPI{
    class Model;
    namespace ModelAssets{
        Model* loadFromRes(std::string _name,std::string _path);
        Model* get(std::string _name);
        void deleteOne(std::string _name);
        void deleteAll();
    }
}