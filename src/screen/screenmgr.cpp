#include <LiteGL/screen/screenmgr.hpp>
#include <LiteGL/screen/screenitems.hpp>
#include <LiteGL/window/window.hpp>
#include <LiteGL/graphics/shader.hpp>
#include <LiteGL/graphics/vertexarray.hpp>
#include <LiteGL/graphics/shaderconstructor.hpp>
#include <LiteGL/graphics/mesh.hpp>
#include <LiteGL/graphics/texture.hpp>
#include <LiteGL/assets/tb.hpp>
#include <LiteGL/assets/scrbuffer.hpp>
#include "../system/priv_logger.hpp"
#include <glm/ext.hpp>
#include "../system/priv_cache.hpp"
#include "./screen_shaders.hpp"
#include <vector>
#include <set>
#include <memory>
#include "../graphics/fontloader.hpp"

#define _ITEM_MESH_INIT_COLOR float r((float)color.r / 255),g((float)color.g / 255),b((float)color.b / 255),a((float)color.a / 255);
#define _ITEM_MESH_INIT_POSITION float x1(position.x + (window_size.x * relative.x)),x2(x1+size.x),y1(position.y + (window_size.y * relative.y)),y2(y1+size.y);
#define _ITEM_MESH_INIT_UV float u1(uv[0]),v1(uv[1]),u2(uv[2]),v2(uv[3]);

using PRIV::GlyphMetaData;

namespace{
    vector2<uint16> window_size;
    struct ScreenItemRenderInfo{
        unsigned vertex_count{0},vertex_offset{0};
        LiteAPI::ScreenItem *item;
        LiteAPI::Primitive primitive;
        LiteAPI::ScreenItemState *state;
        LiteAPI::Shader *shadertype;
        std::string texture_key;
    };
    LiteAPI::Shader *nontex_shader,*tex_shader,*text_shader;
    LiteAPI::Screen* current = nullptr;
    void xuirecompile_shaders(){
        system_logger->info() << ("Recompiling shader");
        LiteAPI::ShaderConstructor ct;
        ct.addFromString(ENGINE_RES::rect_code_frag,LiteAPI::ShaderType::Fragment);
        ct.addFromString(ENGINE_RES::rect_code_vert,LiteAPI::ShaderType::Vertex);
        nontex_shader = ct.create();
        ct.clear();
        Cache::cache_shader(nontex_shader,"nontex_shader");
        ct.addFromString(ENGINE_RES::tex_code_frag,LiteAPI::ShaderType::Fragment);
        ct.addFromString(ENGINE_RES::tex_code_vert,LiteAPI::ShaderType::Vertex);
        tex_shader = ct.create();
        ct.clear();
        Cache::cache_shader(tex_shader,"tex_shader");
        ct.addFromString(ENGINE_RES::text_code_frag,LiteAPI::ShaderType::Fragment);
        ct.addFromString(ENGINE_RES::text_code_vert,LiteAPI::ShaderType::Vertex);
        text_shader = ct.create();
        ct.clear();
        Cache::cache_shader(text_shader,"text_shader");
    }
    glm::mat4 screenView;
}
namespace ScreenData{
    LiteAPI::DynamicMesh *nontext_mesh,*text_mesh,*tex_mesh;
    LiteAPI::VertexArray *nontext_arr,*text_arr,*tex_arr;
    
    std::vector<ScreenItemRenderInfo> screen_item_render_info;
}

namespace LiteAPI{
    void createItemData(ScreenItem *item){
        ScreenItemRenderInfo data;
        data.item = item;
        
        VertexArray *arr = item->getMesh();
        data.primitive = item->getPrimitive();
        data.vertex_count = arr->getVertexCount();
        data.state = &item->getItemState();
        *data.state = ScreenItemState::Idle;


        switch(item->getType()){
            case ScreenItemType::Rectangle:
            case ScreenItemType::Line:
            data.vertex_offset = ScreenData::nontext_arr->getVertexCount();
            data.shadertype=nontex_shader;
            data.texture_key="";
            ScreenData::screen_item_render_info.push_back(data);
            ScreenData::nontext_arr->insert(arr->getData(),arr->getVertexCount());
            break;
            case ScreenItemType::Texture:
            data.vertex_offset = ScreenData::tex_arr->getVertexCount();
            data.shadertype=tex_shader;{
                TextureItem *tex_item = static_cast<TextureItem*>(item);
                data.texture_key=tex_item->getTextureKey();
            }
            ScreenData::screen_item_render_info.push_back(data);
            ScreenData::tex_arr->insert(arr->getData(),arr->getVertexCount());
            break;
            case ScreenItemType::Text:
            data.vertex_offset = ScreenData::text_arr->getVertexCount();
            data.shadertype=text_shader;
            data.texture_key="__font__";
            ScreenData::screen_item_render_info.push_back(data);
            ScreenData::text_arr->insert(arr->getData(),arr->getVertexCount());
            break;
        }
        delete arr;
    }


    ScreenItem::ScreenItem(vector2<int> a, color4 c,vector2<float> r):position(a),color(c),relative(r){}
    vector2<int> ScreenItem::getPosition(){
        return position;
    }
    color4 ScreenItem::getColor(){
        return color;
    }
    ScreenItemState& ScreenItem::getItemState(){
        return state;
    }
    void ScreenItem::setColor(color4 _a){
        state=ScreenItemState::Modified;
        color=_a;
    }
    void ScreenItem::setPosition(vector2<int> _a){
        state=ScreenItemState::Modified;
        position=_a;
    }
    vector2<float> ScreenItem::getRelative(){
        return relative;
    }
    void ScreenItem::setRelative(vector2<float> _r){
        relative = _r;
    }
    bool ScreenItem::relativeIsZero(){
        return relative.x == 0 && relative.y == 0;
    }


    RectangleItem::RectangleItem(vector2<int> a,vector2<unsigned> b,color4 c,vector2<float> r):ScreenItem(a,c,r),size(b){}
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

    LineItem::LineItem(vector2<int> a,vector2<int> b,color4 c,vector2<float> r):ScreenItem(a,c,r),position2(b){}
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

    TextureItem::TextureItem(vector2<int> a,color4 c,vector2<unsigned> b,std::string _tex_key,vector2<float> uv1,vector2<float> uv2,vector2<float> r):ScreenItem(a,c,r),size(b),uv(new float[4]),texture_key(_tex_key){
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
        VertexArray *arr = new VertexArray(8);
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
    std::string TextureItem::getTextureKey(){
        return texture_key;
    }
    void TextureItem::setTextureKey(std::string _key){
        texture_key = _key;
    }
    
    TextItem::TextItem(vector2<int> a,color4 b,std::wstring c,float s,vector2<float> r):ScreenItem(a,b,r),str(c),scale(s){}
    VertexArray* TextItem::getMesh(){
        _ITEM_MESH_INIT_COLOR
        VertexArray *arr = new VertexArray(8);
        float xpos = 0;
        for(wchar_t &ch : str){
            GlyphMetaData data = PRIV::FontLoader::getGlyphMetaData(ch);
            float x1 = position.x + xpos + data.bearing.x  + (window_size.x * relative.x);
            float y1 = position.y + (*PRIV::FontLoader::character_size_ptr - data.bearing.y) * scale  + (window_size.y * relative.y);
            float x2 = x1 + data.size.x * scale;
            float y2 = y1 + data.size.y * scale;
            
            float u1 = data.texCoord.x;
            float v1 = data.texCoord.y;
            float u2 = data.texCoord.x + data.texSize.x;
            float v2 = data.texCoord.y + data.texSize.y;

            float vertices[] = {
                x1,y1,r,g,b,a,u1,v1,
                x2,y1,r,g,b,a,u2,v1,
                x1,y2,r,g,b,a,u1,v2,
                x2,y1,r,g,b,a,u2,v1,
                x2,y2,r,g,b,a,u2,v2,
                x1,y2,r,g,b,a,u1,v2
            };
            arr->insert(vertices,6);
            xpos += data.advance * scale;
        }
        return arr;
    }
    std::wstring TextItem::getString(){
        return str;
    }
    void TextItem::setString(std::wstring str){
        this->str = str;
        state=ScreenItemState::Modified;
    }
    float TextItem::getTextScale(){
        return scale;
    }
    void TextItem::setTextScale(float scale){
        this->scale = scale;
        state=ScreenItemState::Modified;
    }


    Screen::~Screen(){
        for(auto it : items){
            delete it.second;
        }
    }

    void Screen::parseData(){
        ScreenData::nontext_arr->clear();
        ScreenData::text_arr->clear();
        ScreenData::screen_item_render_info.clear();

        for(auto &it : this->items){
            ScreenItem *item = it.second;
            createItemData(item);
        }

        ScreenData::nontext_mesh->reload(ScreenData::nontext_arr);
        ScreenData::text_mesh->reload(ScreenData::text_arr);
        ScreenData::tex_mesh->reload(ScreenData::tex_arr);
    }

    void Screen::update(){
        int totaloffset = 0;
        for(auto &info : ScreenData::screen_item_render_info){
            ScreenItem* item = info.item;
            if(item->getType()==ScreenItemType::Text){
                info.vertex_offset += totaloffset;
            }
            if(*info.state == ScreenItemState::Modified){
                VertexArray *array = item->getMesh();
                switch(item->getType()){
                    case ScreenItemType::Line:
                    case ScreenItemType::Rectangle:
                    ScreenData::nontext_arr->replace(array->getData(),info.vertex_offset,info.vertex_count);
                    break;
                    case ScreenItemType::Texture:
                    ScreenData::tex_arr->replace(array->getData(),info.vertex_offset,info.vertex_count);
                    break;
                    case ScreenItemType::Text:{
                        unsigned newsize = array->getVertexCount();
                        int delta = newsize - info.vertex_count;
                        if(delta == 0){
                            ScreenData::text_arr->replace(array->getData(),info.vertex_offset,info.vertex_count);
                        }else{
                            ScreenData::text_arr->erase(info.vertex_count,info.vertex_offset);
                            ScreenData::text_arr->insert(array->getData(),newsize,info.vertex_offset);
                            info.vertex_count = newsize;
                            totaloffset+=delta;
                        }
                    }
                    break;
                }
                delete array;
                *info.state = ScreenItemState::Idle;
            }
        }
        for(auto &it : this->items){
            ScreenItem *item = it.second;
            if(item->getItemState() == ScreenItemState::Created){createItemData(item);}
        }
        ScreenData::nontext_mesh->reload(ScreenData::nontext_arr);
        ScreenData::text_mesh->reload(ScreenData::text_arr);
        ScreenData::tex_mesh->reload(ScreenData::tex_arr);
    }
    void Screen::updateRelatived(){
        int totaloffset = 0;
        for(auto &info : ScreenData::screen_item_render_info){
            ScreenItem* item = info.item;
            if(item->getType()==ScreenItemType::Text){
                info.vertex_offset += totaloffset;
            }
            if(!item->relativeIsZero()){
                VertexArray *array = item->getMesh();
                switch(item->getType()){
                    case ScreenItemType::Line:
                    case ScreenItemType::Rectangle:
                    ScreenData::nontext_arr->replace(array->getData(),info.vertex_offset,info.vertex_count);
                    break;
                    case ScreenItemType::Texture:
                    ScreenData::tex_arr->replace(array->getData(),info.vertex_offset,info.vertex_count);
                    break;
                    case ScreenItemType::Text:{
                        unsigned newsize = array->getVertexCount();
                        int delta = newsize - info.vertex_count;
                        if(delta == 0){
                            ScreenData::text_arr->replace(array->getData(),info.vertex_offset,info.vertex_count);
                        }else{
                            ScreenData::text_arr->erase(info.vertex_count,info.vertex_offset);
                            ScreenData::text_arr->insert(array->getData(),newsize,info.vertex_offset);
                            info.vertex_count = newsize;
                            totaloffset+=delta;
                        }
                    }
                    break;
                }
                delete array;
                *info.state = ScreenItemState::Idle;
            }
        }
        ScreenData::nontext_mesh->reload(ScreenData::nontext_arr);
        ScreenData::text_mesh->reload(ScreenData::text_arr);
        ScreenData::tex_mesh->reload(ScreenData::tex_arr);
    }

    void Screen::addItem(std::string _name,ScreenItem *_item){
        for(auto i = items.begin(); i!=items.end();++i){
            if(_name == i->first){
                delete i->second;
                items.erase(i);
                break;
            }
        }
        items.emplace_back(_name,_item);
    }

    ScreenItem* Screen::getItem(std::string _name){
        for(auto i = items.begin(); i!=items.end();++i){
            if(_name == i->first){
                return i->second;
            }
        }
        return nullptr;
    }

    namespace ScreenMGR{
        const glm::mat4 &screenView(screenView);
        void setScreen(std::string _name){
            system_logger->info() << "Setup screendata for " << _name;
            current = ScreenAssets::get(_name);
            current->parseData();
        };
        void updateScreen(){
            current->update();
        }
        void renderScreen(){
            for(auto &info : ScreenData::screen_item_render_info){
                info.shadertype->bind();
                info.shadertype->uniformMatrix("view",::screenView);
                if(info.texture_key.empty()){
                    ScreenData::nontext_mesh->drawPart(info.primitive,info.vertex_count,info.vertex_offset);
                }else if(info.texture_key=="__font__"){
                    PRIV::FontLoader::glyph_atlas_ptr->use();
                    ScreenData::text_mesh->drawPart(info.primitive,info.vertex_count,info.vertex_offset);
                }else{
                    TextureAssets::get(info.texture_key)->use();
                    ScreenData::tex_mesh->drawPart(info.primitive,info.vertex_count,info.vertex_offset);
                }
            }
        }
        Screen* getCurrentScreen(){
            return current;
        }
    }
}
namespace PRIV{
	namespace ScreenMGR{
        void recalc_screenView(){
            window_size = LiteAPI::Window::getSize();
            glm::mat4 mat = glm::ortho(0.f,(float)window_size.x,(float)window_size.y,0.f);
            screenView = mat;
            current->updateRelatived();
        }
        void recalc_screenView_noupdate(){
            window_size = LiteAPI::Window::getSize();
            glm::mat4 mat = glm::ortho(0.f,(float)window_size.x,(float)window_size.y,0.f);
            screenView = mat;
        }
        void initialize(){
            ScreenData::nontext_mesh = new LiteAPI::DynamicMesh(nullptr,0,(int[]){2,4,0});
            ScreenData::text_mesh = new LiteAPI::DynamicMesh(nullptr,0,(int[]){2,4,2,0});
            ScreenData::tex_mesh = new LiteAPI::DynamicMesh(nullptr,0,(int[]){2,4,2,0});
            ScreenData::nontext_arr = new LiteAPI::VertexArray(6);
            ScreenData::text_arr = new LiteAPI::VertexArray(8);
            ScreenData::tex_arr = new LiteAPI::VertexArray(8);

            system_logger->info() << "ScreenMGR initializing...";
            nontex_shader = Cache::load_chached_shader("nontex_shader");
            tex_shader = Cache::load_chached_shader("tex_shader");
            text_shader = Cache::load_chached_shader("text_shader");
            if(!(nontex_shader) || !(tex_shader) || !(text_shader)){
                xuirecompile_shaders();
            }
            
            system_logger->info() << "ScreenMGR initalized!";
        }
        void finalize(){
            delete nontex_shader;
            delete tex_shader;
            delete text_shader;
            
            delete ScreenData::nontext_arr;
            delete ScreenData::tex_arr;
            delete ScreenData::text_arr;
            
            delete ScreenData::nontext_mesh;
            delete ScreenData::tex_mesh;
            delete ScreenData::text_mesh;
        }
	}
}
