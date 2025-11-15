#include <fstream>
#include "fontloader.hpp"
#include <vector>
#include <LiteGL/graphics/texture.hpp>
#include <map>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../LiteData.hpp"

#define F_TOCHAR(OBJ) reinterpret_cast<char*>(&OBJ)

namespace{
    struct Locale{
        unsigned offset;
        unsigned size;
    };

    FT_Library ft;
    FT_Face face;
    LiteAPI::Texture* glyph_atlas;
    std::map<unsigned,PRIV::GlyphMetaData> glyphs;

    //Locale structure: [locales count:unsigned][offset:unsigned][size:unsiged]...
    std::vector<Locale> load_locale(){
        std::ifstream file("./res/locale.loc",std::ios::binary);
        if(!file.is_open())throw std::runtime_error("Couldn't load locale file in ./res/locale.loc");
        unsigned locales_count;file.read(F_TOCHAR(locales_count),sizeof(unsigned));
        std::vector<Locale> tmp;
        for(unsigned i = 0;i<locales_count;i++){
            Locale loc;
            file.read(F_TOCHAR(loc.offset),sizeof(unsigned));
            file.read(F_TOCHAR(loc.size),sizeof(unsigned));
            tmp.push_back(loc);
        }
        file.close();
        return tmp;
    }
    void create_atlas(){
        const LiteAPI::INISection& fontloader_config = (*LiteDATA::main_config)["fontloader"];
        const uint32 glyph_atlas_size = std::stoi(fontloader_config["glyph_atlas_size"]);
        const uint8 character_scale = std::stoi(fontloader_config["character_scale"]);
        FT_Set_Pixel_Sizes(face,0,character_scale);
        std::vector<Locale> locale = load_locale();
        uint8 *atlasData = new uint8[glyph_atlas_size*glyph_atlas_size];
        memset(atlasData,0,glyph_atlas_size*glyph_atlas_size);
        unsigned x=0, y=0, maxrow=0;
        for(const Locale &locale : locale){
            for(unsigned i = 0;i<locale.size;i++){
                if(FT_Load_Char(face,i+locale.offset,FT_LOAD_RENDER))continue;
                const FT_Bitmap bitmap = face->glyph->bitmap;
                if(x+bitmap.width >= glyph_atlas_size){
                    x = 0;  
                    y += maxrow;
                    maxrow=0;
                }
                if(y+bitmap.rows>=glyph_atlas_size){
                    throw std::runtime_error("Fontloader OUT OF MEMORY");
                }
                for(int row = 0;row < bitmap.rows;row++){
                    for(int col = 0;col < bitmap.width;col++){
                        unsigned char value = bitmap.buffer[row*bitmap.width+col];
                        int index = ((y+row)*glyph_atlas_size+(x+col));
                        atlasData[index]=value;
                    }
                }
                PRIV::GlyphMetaData data;
                data.advance = face->glyph->advance.x >> 6;
                data.texCoord = {(float)x/glyph_atlas_size , (float)y/glyph_atlas_size};
                data.texSize = {(float)bitmap.width/glyph_atlas_size,(float)bitmap.rows/glyph_atlas_size};
                data.size = {bitmap.width,bitmap.rows};
                data.bearing = {face->glyph->bitmap_left,face->glyph->bitmap_top};
                glyphs[i+locale.offset] = data;
                x += bitmap.width;
                maxrow = std::max(maxrow,(unsigned)bitmap.rows);
            }
        }
        unsigned id;
        glGenTextures(1,&id);
        glBindTexture(GL_TEXTURE_2D,id);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            glyph_atlas_size,
            glyph_atlas_size,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            atlasData
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glyph_atlas = new LiteAPI::Texture(id);
        delete[] atlasData;
    }
}


namespace PRIV{
    namespace FontLoader{
        const LiteAPI::Texture *glyph_atlas_ptr = nullptr;
        void loadfrom(std::string _path){
            if(FT_Init_FreeType(&ft))throw std::runtime_error("coudn't initialize freetype");
            if(FT_New_Face(ft,_path.c_str(),0,&face))throw std::runtime_error("coudn't load font: "+_path);
            create_atlas();
            glyph_atlas_ptr=glyph_atlas;
        }
        const GlyphMetaData& getGlyphMetaData(unsigned data){
            auto it = glyphs.find(data);
            if(it==glyphs.end()){
                throw std::runtime_error("Couldn't find glyph data! Check res/locale.loc.");
            }
            return it->second;
        }
        void close(){
            FT_Done_Face(face);
            FT_Done_FreeType(ft);
        }
    }
}