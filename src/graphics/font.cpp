#include <fstream>
#include <LiteGL/graphics/font.hpp>
#include <vector>
#include <LiteGL/graphics/texture.hpp>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../LiteData.hpp"
#include <filesystem>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <stb_image.h>
#include "../system/priv_logger.hpp"
#include "../system/priv_cache.hpp"

#define F_TOCHAR(OBJ) reinterpret_cast<char*>(&OBJ)
#define F_TOCHAR_NOPTR(OBJ) reinterpret_cast<char*>(OBJ)

using GlyphMetaData = LiteAPI::Font::GlyphMetaData;

namespace{
    struct Locale{
        unsigned offset;
        unsigned size;
    };

    FT_Library ft;
    uint8 character_scale;
    std::vector<Locale> locale;

    const LiteAPI::INISection* fontloader_config;

    void load_locale(){
        locale.clear();
        std::ifstream file("./res/locale.loc",std::ios::binary);
        if(!file.is_open())throw std::runtime_error("Couldn't load locale file in ./res/locale.loc");
        unsigned locales_count;file.read(F_TOCHAR(locales_count),sizeof(unsigned));
        for(unsigned i = 0;i<locales_count;i++){
            Locale loc;
            file.read(F_TOCHAR(loc.offset),sizeof(unsigned));
            file.read(F_TOCHAR(loc.size),sizeof(unsigned));
            locale.push_back(loc);
        }
        file.close();
    }
    void export_to_png(std::string name,uint8 *data,const unsigned &glyph_atlas_size){
        stbi_write_png((".cache/fontatlas"+name+".png").c_str(),glyph_atlas_size,glyph_atlas_size,1,data,glyph_atlas_size);
        system_logger->info() << "Exported .cache/font_atlas.png";
    }
    uint8* import_from_png(std::string name, int* width, int* height) {
        int channels;
        std::string filename = ".cache/fontatlas"+name+".png";
        uint8* data = stbi_load(filename.c_str(), width, height, &channels, 1);
        if (!data) {
            throw std::runtime_error(std::string("Failed to load PNG: ") + filename);
        }
        return data;
    }
    
    bool check_locale_data(std::string name){
        return std::filesystem::exists(".cache/fontdata"+name+".bin");
    }
}

namespace LiteAPI{
    Font::Font(std::filesystem::path path):font_name(Cache::genHash(path.filename().string())){
        if(check_locale_data(font_name)){
            loadFontData();
        }else{
            genFontData(path);
        }
    }
    Font::~Font(){
        delete this->glyph_atlas;
    }

    Texture* Font::getTexture(){
        return this->glyph_atlas;
    }
    const Font::GlyphMetaData& Font::operator[](wchar_t ch){
        return glyphs[ch];
    }
    void Font::genFontData(std::filesystem::path path){
        FT_Face face;
        if(FT_New_Face(ft,path.c_str(),0,&face))throw std::runtime_error("coudn't load font: "+path.string());
        unsigned glyph_atlas_size = std::stoi((*fontloader_config)["glyph_atlas_size"]);
        character_heigth = std::stoi((*fontloader_config)["character_scale"]);

        FT_Set_Pixel_Sizes(face,0,character_scale);
        uint8 *atlasData = new uint8[glyph_atlas_size*glyph_atlas_size];
        memset(atlasData,0,glyph_atlas_size*glyph_atlas_size);
        unsigned x=0, y=0, maxrow=0;
        for(const Locale &locale : locale){
            for(unsigned i = 0;i<locale.size;i++){
                if(FT_Load_Char(face,i+locale.offset,FT_LOAD_RENDER)){
                    system_logger->warn() << "Character not found: " <<  i + locale.offset;
                    continue;
                }
                const FT_Bitmap bitmap = face->glyph->bitmap;
                if(x+bitmap.width+1 >= glyph_atlas_size){
                    x = 0;  
                    y += maxrow+1;
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
                GlyphMetaData data;
                data.advance = face->glyph->advance.x >> 6;
                data.texCoord = {(float)x/glyph_atlas_size , (float)y/glyph_atlas_size};
                data.texSize = {(float)bitmap.width/glyph_atlas_size,(float)bitmap.rows/glyph_atlas_size};
                data.size = {bitmap.width,bitmap.rows};
                data.bearing = {face->glyph->bitmap_left,face->glyph->bitmap_top};
                wchar_t ch = i + locale.offset;
                glyphs[ch] = data;
                x += bitmap.width+1;
                maxrow = std::max(maxrow,(unsigned)bitmap.rows);
            }
        }
        createAtlas(atlasData,glyph_atlas_size);
        saveFontData(atlasData,glyph_atlas_size);
        delete[] atlasData;
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }
    void Font::createAtlas(uint8 *data,const unsigned &glyph_atlas_size){
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
            data
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glyph_atlas = new LiteAPI::Texture(id);
    }
    void Font::loadFontData(){
        std::ifstream file(".cache/fontdata"+font_name+".bin",std::ios::binary);
        if(!file.is_open())throw std::runtime_error("Coudn't load cached font data!");
        unsigned count;
        file.read(F_TOCHAR(character_scale),sizeof(unsigned));
        file.read(F_TOCHAR(count),sizeof(unsigned));
        for(unsigned i = 0;i<count;i++){
            unsigned character;
            file.read(F_TOCHAR(character),sizeof(unsigned));

            LiteAPI::Font::GlyphMetaData data;
            file.read(F_TOCHAR(data),sizeof(LiteAPI::Font::GlyphMetaData));

            glyphs[character] = data;
        }
        uint32 atlasSize;
        file.read(F_TOCHAR(atlasSize),sizeof(uint32));
        file.read(F_TOCHAR(character_heigth),sizeof(uint32));
        file.close();

        int w,h;
        uint8* data = import_from_png(font_name, &w, &h);
        if (w != atlasSize || h != atlasSize) {
            stbi_image_free(data);
            throw std::runtime_error("FontLoader: Cached PNG size doesn't match metadata");
        }
        createAtlas(data,atlasSize);
        delete[] data;
    }
    void Font::saveFontData(uint8 *data,const unsigned &glyph_atlas_size){
        std::ofstream file(".cache/fontdata"+font_name+".bin",std::ios::binary);
        if(!file.is_open())throw std::runtime_error("Coudn't cache font data!");
        file.write(F_TOCHAR(character_scale),sizeof(unsigned));
        unsigned count = glyphs.size();
        file.write(F_TOCHAR(count),sizeof(unsigned));
        for(auto &it : glyphs){
            unsigned character = it.first;
            GlyphMetaData data = it.second;

            file.write(F_TOCHAR(character),sizeof(unsigned));
            file.write(F_TOCHAR(data),sizeof(GlyphMetaData));
        }
        unsigned tmp = glyph_atlas_size;
        file.write(F_TOCHAR(tmp),sizeof(uint32));
        file.write(F_TOCHAR(character_heigth),sizeof(uint32));
        export_to_png(font_name,data,glyph_atlas_size);
        system_logger->info() << "Created new cached font data";
    }
    unsigned Font::getGlyphHeight(){
        return this->character_heigth;
    }
}

namespace PRIV{

    namespace FontLoader{ 
        void init(){
            if(FT_Init_FreeType(&ft))throw std::runtime_error("FontLoader: Coudn't init freetype");
            fontloader_config = &(*LiteDATA::main_config)["fontloader"];
            load_locale();
        }
        void close(){
            FT_Done_FreeType(ft);
        }
    }
}