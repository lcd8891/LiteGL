#include <LiteGL/screen/screenmgr.hpp>
#include <LiteGL/window/window.hpp>
#include <LiteGL/graphics/shader.hpp>
#include <LiteGL/graphics/shaderconstructor.hpp>
#define LOGGER_GROUP "screenmgr"
#include <LiteGL/logger.hpp>
#include <glm/ext.hpp>
#include "../priv/cache.hpp"
#include "./screen_shaders.hpp"

namespace{
    LiteAPI::Shader *rectShader,*texShader;
    void recompile_shaders(){
        LiteAPI::Logger::info("Recompiling shader");
        LiteAPI::ShaderConstructor ct;
        ct.addFromString(ENGINE_RES::rect_code_frag,LiteAPI::ShaderType::Fragment);
        ct.addFromString(ENGINE_RES::rect_code_vert,LiteAPI::ShaderType::Vertex);
        rectShader = ct.create();
        ct.clear();
        
        Cache::save_shader("rectshader",rectShader);
    }
}

namespace LiteAPI{
    namespace Screenmgr{
        glm::mat4 screenView;
        void recalc_screenView(){
            vector2<uint16> size = Window::getSize();
            glm::mat4 mat = glm::translate(glm::mat4(1.f),glm::vec3(-1,1,0));
            mat = glm::scale(mat,glm::vec3(2.f / size.x, 2.f / size.y, 1));
            screenView = mat;
        }
        void initialize(){
            Logger::info("ScreenMGR initializing...");
            rectShader = Cache::load_shader("rectshader");
            if(!(rectShader && texShader)){
                recompile_shaders();
            }
            Logger::info("ScreenMGR initalized!");
        }
        void finalize(){
            delete rectShader;
        }
    }
}