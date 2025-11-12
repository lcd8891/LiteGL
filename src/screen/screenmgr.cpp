#include <LiteGL/screen/screenmgr.hpp>
#include <LiteGL/screen/screenitems.hpp>
#include <LiteGL/window/window.hpp>
#include <LiteGL/graphics/shader.hpp>
#include <LiteGL/graphics/vertexarray.hpp>
#include <LiteGL/graphics/shaderconstructor.hpp>
#include <LiteGL/graphics/mesh.hpp>
#define LOGGER_GROUP "screenmgr"
#include "../system/priv_logger.hpp"
#include <LiteGL/buffer/scrbuffer.hpp>
#include <glm/ext.hpp>
#include "../system/priv_cache.hpp"
#include "./screen_shaders.hpp"
#include <vector>
#include <set>

#define _V_ADD_NONTEXT() 

namespace ScreenData{
    LiteAPI::DynamicMesh *nontext_mesh,*text_mesh;
    LiteAPI::VertexArray *nontext_arr;
    std::unordered_map scritemsdata;
}
namespace{
    class ScreenItemData{
        glm::mat4 matrix;
        LiteAPI::VertexArray::VertexIterator *vertices;
    };
    LiteAPI::Shader *rectShader,*texShader;
    LiteAPI::Screen* current;
    void recompile_shaders(){
        system_logger->info() << ("Recompiling shader");
        LiteAPI::ShaderConstructor ct;
        ct.addFromString(ENGINE_RES::rect_code_frag,LiteAPI::ShaderType::Fragment);
        ct.addFromString(ENGINE_RES::rect_code_vert,LiteAPI::ShaderType::Vertex);
        rectShader = ct.create();
        ct.clear();
        
        Cache::cache_shader(rectShader,"rectshader");
    }
}

namespace LiteAPI{
    Screen::~Screen(){
        for(auto it : items){
            delete it.second;
        }
    }
    void Screen::operator()(){
        if(ScreenData::nontext_arr)delete ScreenData::nontext_arr;
        ScreenData::nontext_arr = new LiteAPI::VertexArray(6);
        for(auto &it : this->items){
            ScreenItem *item = it.second;
            
        }
    }
    void Screen::update(){
        
    }
    void Screen::add_item(std::string _name,ScreenItem *_item){
        items[_name]=_item;
    }
    ScreenItem* Screen::getItem(std::string _name){
        auto it = items.find(_name);
        return (it->second) ? it->second : nullptr;
    }
    namespace ScreenMGR{
        glm::mat4 screenView;
        void set_screen(std::string _name){
            current = ScreenBuffer::get_screen(_name);
            (*current)();
        };
        void update_screen(){
            current->update();
        }
        void render_screen(){
            rectShader->bind();
            rectShader->uniformMatrix("view",screenView);
        }
    }
}
namespace PRIV{
	namespace ScreenMGR{
        void recalc_screenView(){
            vector2<uint16> size = LiteAPI::Window::getSize();
            glm::mat4 mat = glm::translate(glm::mat4(1.f),glm::vec3(-1,1,0));
            mat = glm::scale(mat,glm::vec3(2.f / size.x, 2.f / size.y, 1));
            LiteAPI::ScreenMGR::screenView = mat;
        }
        void initialize(){
            ScreenData::nontext_mesh = new LiteAPI::DynamicMesh(nullptr,0,(int[]){2,4});
            ScreenData::text_mesh = new LiteAPI::DynamicMesh(nullptr,0,(int[]){2,2,4});
            system_logger->info() << "ScreenMGR initializing...";
            rectShader = Cache::load_chached_shader("rectshader");
            if(!(rectShader)){
                recompile_shaders();
            }
            system_logger->info() << "ScreenMGR initalized!";
        }
        void finalize(){
            delete rectShader;
        }
	}
}
