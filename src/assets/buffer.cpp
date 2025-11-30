#include <LiteGL/buffer/buffers.hpp>
#include <LiteGL/graphics/graphics.hpp>
#include <LiteGL/screen/screenmgr.hpp>
#include <LiteGL/graphics/model.hpp>

#include "../system/priv_logger.hpp"
#include "../graphics/graphics_tools.hpp"

#include <unordered_map>
#include <stdexcept>

namespace{
    std::unordered_map<std::string,LiteAPI::Texture*> texture_map;
    std::unordered_map<std::string,LiteAPI::Shader*> shader_map;
    std::unordered_map<std::string,LiteAPI::Screen*> screen_map;
    std::unordered_map<std::string,LiteAPI::Model*> model_map;
}

#define _B_NEW(BUF,OBJ) auto it = BUF.find(_name); if(it == BUF.end()){BUF[_name] = OBJ;}else{delete it->second;it->second = OBJ;system_logger->warn() << ("overriding resource: "+_name);} return OBJ;
#define _B_GET(BUF) auto it = BUF.find(_name);if(it == BUF.end()){throw std::runtime_error("trying to get unknown resource: "+_name);}else{return it->second;}
#define _B_DEL(BUF) auto it = BUF.find(_name);if(it == BUF.end()){system_logger->warn() << ("trying to delete unknown resource: "+_name);}else{delete it->second;BUF.erase(_name);}
#define _B_CLR(BUF) for(auto &it : BUF){delete it.second;}BUF.clear();

namespace LiteAPI{
    namespace TextureAssets{
        Texture* loadFromRes(std::string _path, std::string _name){
            Texture* texture = _load_texture_from_file("./res/textures/"+_path+".png");
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
    namespace ModelAssets{
        Model* loadFromRes(std::string _name,std::string _path){
            Model* model = new Model(_path);
            _B_NEW(model_map,model);
        }
        Model* get(std::string _name){
            _B_GET(model_map);
        }
        void deleteOne(std::string _name){
            _B_DEL(model_map);
        }
        void deleteAll(){
            _B_CLR(model_map);
        }
    }
}
namespace PRIV{
    void texture_buffer_set_mem(LiteAPI::Texture *obj,std::string _name){
        texture_map[_name] = obj;
    }
}