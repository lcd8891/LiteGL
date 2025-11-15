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
#include <memory>

#define _ITEM_MESH_INIT_COLOR float r(color.r),g(color.g),b(color.b),a(color.a);
#define _ITEM_MESH_INIT_POSITION float x1(position.x),x2(position.x+size.x),y1(position.y),y2(position.y+size.y);
#define _ITEM_MESH_INIT_UV float u1(uv[0]),v1(uv[1]),u2(uv[2]),v2(uv[3]);

namespace{
    struct ScreenItemRenderInfo{
        unsigned vertex_count{0},vertex_offset{0};
        LiteAPI::ScreenItem *item;
        LiteAPI::Primitive primitive;
        const bool *modified;
    };
    LiteAPI::Shader *nontex_shader,*tex_shader;
    LiteAPI::Screen* current;
    void recompile_shaders(){
        system_logger->info() << ("Recompiling shader");
        LiteAPI::ShaderConstructor ct;
        ct.addFromString(ENGINE_RES::rect_code_frag,LiteAPI::ShaderType::Fragment);
        ct.addFromString(ENGINE_RES::rect_code_vert,LiteAPI::ShaderType::Vertex);
        nontex_shader = ct.create();
        ct.clear();
        Cache::cache_shader(nontex_shader,"nontex_shader");
    }
    glm::mat4 screenView;
}
namespace ScreenData{
    LiteAPI::DynamicMesh *nontext_mesh,*text_mesh;
    LiteAPI::VertexArray *nontext_arr,*text_arr;
    
    std::unordered_map<std::string,ScreenItemRenderInfo> screen_item_render_info;
}

namespace LiteAPI{
    ScreenItem::ScreenItem(vector2<int> a, color4 b):position(a),color(b){}
    vector2<int> ScreenItem::getPosition(){
        return position;
    }
    color4 ScreenItem::getColor(){
        return color;
    }
    const bool& ScreenItem::getModified(){
        return modified;
    }
    void ScreenItem::setColor(color4 _a){
        modified=true;
        color=_a;
    }
    void ScreenItem::setPosition(vector2<int> _a){
        modified=true;
        position=_a;
    }


    RectangleItem::RectangleItem(vector2<int> a,vector2<unsigned> b,color4 c):ScreenItem(a,c),size(b){}
    VertexArray* RectangleItem::getMesh(){
        _ITEM_MESH_INIT_COLOR
        _ITEM_MESH_INIT_POSITION
        float vertices[] = {
            x1, y1, r, g, b, a, 
            x2, y1, r, g, b, a,   
            x1, y2, r, g, b, a, 
            x2, y1, r, g, b, a, 
            x2, y2, r, g, b, a, 
            x1, y2, r, g, b, a  
        };
            VertexArray *arr = new VertexArray(6);
            arr->insert(vertices,6);
            return arr;
    }

    LineItem::LineItem(vector2<int> a,vector2<int> b,color4 c):ScreenItem(a,c),position2(b){}
    VertexArray* LineItem::getMesh(){
        _ITEM_MESH_INIT_COLOR
        float x1 = static_cast<float>(position.x);
        float y1 = static_cast<float>(position.y);
        float x2 = static_cast<float>(position2.x);
        float y2 = static_cast<float>(position2.y);
        float vertices[] = {
            x1, y1, r, g, b, a, 
            x2, y2, r, g, b, a
        };
        VertexArray *arr = new VertexArray(6);
        arr->insert(vertices,2);
        return arr;
    }

    TextureItem::TextureItem(vector2<int> a,color4 c,vector2<unsigned> b,vector2<float> uv1,vector2<float> uv2):ScreenItem(a,c),size(b),uv(new float[4]){
        uv[0] = uv1.x;
        uv[1] = uv1.y;
        uv[2] = uv2.x;
        uv[3] = uv2.y;
    }
    TextureItem::~TextureItem(){
        delete[] uv;
    }
    VertexArray* TextureItem::getMesh(){
        _ITEM_MESH_INIT_COLOR
        _ITEM_MESH_INIT_POSITION
        _ITEM_MESH_INIT_UV
        float vertices[] = {
            x1, y1, r, g, b, a,u1,v1, 
            x2, y1, r, g, b, a,u2,v1,
            x1, y2, r, g, b, a,u1,v2,
            x2, y1, r, g, b, a,u2,v1,
            x2, y2, r, g, b, a,u2,v2,
            x1, y2, r, g, b, a,u1,v2
        };
        VertexArray *arr = new VertexArray(6);
        arr->insert(vertices,6);
        return arr;
    }
    const float* TextureItem::getUV(){
        return uv;
    }
    void TextureItem::setUV(float u1,float v1,float u2,float v2){
        uv[0] = u1;
        uv[1] = v1;
        uv[2] = u2;
        uv[3] = v2;
    }
    vector2<unsigned> TextureItem::getSize(){
        return size;
    }
    void TextureItem::setSize(vector2<unsigned> a){
        size = a;
    }


    Screen::~Screen(){
        for(auto it : items){
            delete it.second;
        }
    }

    void Screen::operator()(){
        ScreenData::nontext_arr->clear();
        ScreenData::text_arr->clear();

        ScreenData::screen_item_render_info.clear();

        for(auto &it : this->items){
            ScreenItem *item = it.second;
            ScreenItemRenderInfo data;
            data.item = item;
            VertexArray *vertices = item->getMesh();
            std::string id = it.first;
            data.primitive = item->getPrimitive();
            data.vertex_count = vertices->getVertexCount();
            data.modified = &item->getModified();
            if(item->isTextured()){
                data.vertex_offset = ScreenData::text_arr->getVertexCount();
                ScreenData::screen_item_render_info[id] = data;
                ScreenData::text_arr->insert(vertices->getData(),vertices->getVertexCount());
            }else{
                data.vertex_offset = ScreenData::nontext_arr->getVertexCount();
                ScreenData::screen_item_render_info[id] = data;
                ScreenData::nontext_arr->insert(vertices->getData(),vertices->getVertexCount());
            }
            delete vertices;
        }

        ScreenData::nontext_mesh->reload(ScreenData::nontext_arr);
    }

    void Screen::update(){
        for(auto &it : ScreenData::screen_item_render_info){
            const ScreenItemRenderInfo &info = it.second;
            if(!info.modified)continue;
            VertexArray* vertices = info.item->getMesh();
            if(info.item->isTextured()){
                ScreenData::text_arr->replace(vertices->getData(),info.vertex_offset,info.vertex_count);
            }else{
                ScreenData::nontext_arr->replace(vertices->getData(),info.vertex_offset,info.vertex_count);
            }
        }
        ScreenData::nontext_mesh->reload(ScreenData::nontext_arr);
        
    }

    void Screen::add_item(std::string _name,ScreenItem *_item){
        items[_name]=_item;
    }

    ScreenItem* Screen::getItem(std::string _name){
        auto it = items.find(_name);
        return (it->second) ? it->second : nullptr;
    }

    namespace ScreenMGR{
        const glm::mat4 &screenView(screenView);
        void set_screen(std::string _name){
            system_logger->info() << "Setup screendata for " << _name;
            current = ScreenBuffer::get_screen(_name);
            (*current)();
        };
        void update_screen(){
            current->update();
        }
        void render_screen(){
            nontex_shader->bind();
            nontex_shader->uniformMatrix("view",::screenView);
            for(auto it : ScreenData::screen_item_render_info){
                const ScreenItemRenderInfo &info = it.second;
                ScreenData::nontext_mesh->drawPart(info.primitive,info.vertex_count,info.vertex_offset);
            }
        }
    }
}
namespace PRIV{
	namespace ScreenMGR{
        void recalc_screenView(){
            vector2<uint16> size = LiteAPI::Window::getSize();
            glm::mat4 mat = glm::ortho(0.f,(float)size.x,(float)size.y,0.f);
            screenView = mat;
        }
        void initialize(){
            ScreenData::nontext_mesh = new LiteAPI::DynamicMesh(nullptr,0,(int[]){2,4,0});
            ScreenData::text_mesh = new LiteAPI::DynamicMesh(nullptr,0,(int[]){2,2,4,0});
            ScreenData::nontext_arr = new LiteAPI::VertexArray(6);
            ScreenData::text_arr = new LiteAPI::VertexArray(8);
            system_logger->info() << "ScreenMGR initializing...";
            nontex_shader = Cache::load_chached_shader("nontex_shader");
            if(!(nontex_shader)){
                recompile_shaders();
            }
            system_logger->info() << "ScreenMGR initalized!";
        }
        void finalize(){
            delete nontex_shader;
        }
	}
}
