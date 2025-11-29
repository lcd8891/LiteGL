#include <LiteGL/buffer/buffers.hpp>
#include <LiteGL/graphics/graphics.hpp>
#include <LiteGL/screen/screenmgr.hpp>
#define LOGGER_GROUP "buffers"
#include "../system/priv_logger.hpp"
#include <map>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <GL/glew.h>
#include <stdexcept>

namespace{
std::map<std::string,LiteAPI::Texture*> texture_map;
std::map<std::string,LiteAPI::Shader*> shader_map;
std::map<std::string,LiteAPI::Screen*> screen_map;

}

#define _B_NEW(BUF,OBJ) auto it = BUF.find(_name); if(it == BUF.end()){BUF[_name] = OBJ;}else{delete it->second;it->second = OBJ;system_logger->warn() << ("overriding resource: "+_name);} return OBJ;
#define _B_GET(BUF) auto it = BUF.find(_name);if(it == BUF.end()){throw std::runtime_error("trying to get unknown resource: "+_name);}else{return it->second;}
#define _B_DEL(BUF) auto it = BUF.find(_name);if(it == BUF.end()){system_logger->warn() << ("trying to delete unknown resource: "+_name);}else{delete it->second;BUF.erase(_name);}
#define _B_CLR(BUF) for(auto &it : BUF){delete it.second;}BUF.clear();

LiteAPI::Texture* _load_from_file(const std::string _path){
    unsigned int textureID;
    glGenTextures(1,&textureID);
    int width,heigth,component;

    unsigned char* imagedata = stbi_load(_path.c_str(),&width,&heigth,&component,0);
    if(imagedata){
        int format;
        switch(component){
            case 1:
            format = GL_RED;
            break;
            case 3:
            format = GL_RGB;
            break;
            case 4:
            format = GL_RGBA;
        }
        glBindTexture(GL_TEXTURE_2D,textureID);
        glTexImage2D(GL_TEXTURE_2D,0,format,width,heigth,0,format,GL_UNSIGNED_BYTE,imagedata);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        stbi_image_free(imagedata);
        return new LiteAPI::Texture(textureID);
    }else{
        throw std::runtime_error("Couldn't load texture: "+_path);
    }
}

namespace LiteAPI{
    namespace TextureAssets{
        Texture* loadFromRes(std::string _path, std::string _name){
            Texture* texture = _load_from_file("./res/textures/"+_path+".png");
            _B_NEW(texture_map,texture)
        }
        Texture* get(std::string _name){
            _B_GET(texture_map)
        }
        void deleteOne(std::string _name){
            _B_DEL(texture_map)
        }
        void deleteAll(){
            _B_CLR(texture_map)
        }
    }
    namespace ShaderAssets{
        Shader* loadFromConstructor(const ShaderConstructor &_constructor,std::string _name){
            Shader *shader = _constructor.create();
            _B_NEW(shader_map,shader);
        }
        Shader* get(std::string _name){
            _B_GET(shader_map)
        }
        void deleteOne(std::string _name){
            _B_DEL(shader_map)
        }
        void deleteAll(){
            _B_CLR(shader_map)
        }
    }
    namespace ScreenAssets{
        Screen* create(std::string _name){
            Screen* sas = new Screen;
            _B_NEW(screen_map,sas);
        }
        Screen* get(std::string _name){
            _B_GET(screen_map);
        }
        void deleteOne(std::string _name){
            _B_DEL(shader_map);
        }
        void deleteAll(){
            _B_CLR(shader_map);
        }
    }
}
namespace PRIV{
    void texture_buffer_set_mem(LiteAPI::Texture *obj,std::string _name){
        texture_map[_name] = obj;
    }
}