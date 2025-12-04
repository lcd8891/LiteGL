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

#define F_TOCHAR(OBJ) reinterpret_cast<char*>(&OBJ)
#define F_TOCHAR_NOPTR(OBJ) reinterpret_cast<char*>(OBJ)

namespace{
    struct Locale{
        unsigned offset;
        unsigned size;
    };

    const LiteAPI::INISection* fontloader_config;
    void save_locale_data(uint8 *data);
    void create_atlas(uint8 *data);

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
    void load_glyphs(std::string path){
        FT_Library ft;
        FT_Face face;
        if(FT_Init_FreeType(&ft))throw std::runtime_error("coudn't initialize freetype");
        if(FT_New_Face(ft,path.c_str(),0,&face))throw std::runtime_error("coudn't load font: "+path);
        glyph_atlas_size = std::stoi((*fontloader_config)["glyph_atlas_size"]);
        character_scale = std::stoi((*fontloader_config)["character_scale"]);

        FT_Set_Pixel_Sizes(face,0,character_scale);
        std::vector<Locale> locale = load_locale();
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
                PRIV::GlyphMetaData data;
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
        create_atlas(atlasData);
        save_locale_data(atlasData);
        delete[] atlasData;
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }
    void create_atlas(uint8 *data){
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
    void export_to_png(uint8 *data){
        stbi_write_png(".cache/font_atlas.png",glyph_atlas_size,glyph_atlas_size,1,data,glyph_atlas_size);
        system_logger->info() << "Exported .cache/font_atlas.png";
    }
    uint8* import_from_png(const char* filename, int* width, int* height) {
        int channels;
        uint8* data = stbi_load(filename, width, height, &channels, 1);
        if (!data) {
            throw std::runtime_error(std::string("Failed to load PNG: ") + filename);
        }
        return data;
    }
    void save_locale_data(uint8 *data){
        std::ofstream file(".cache/font.bin",std::ios::binary);
        if(!file.is_open())throw std::runtime_error("Coudn't cache font data!");
        file.write(F_TOCHAR(character_scale),sizeof(unsigned));
        unsigned count = glyphs.size();
        file.write(F_TOCHAR(count),sizeof(unsigned));
        for(auto &it : glyphs){
            unsigned character = it.first;
            PRIV::GlyphMetaData data = it.second;

            file.write(F_TOCHAR(character),sizeof(unsigned));
            file.write(F_TOCHAR(data),sizeof(PRIV::GlyphMetaData));
        }
        file.write(F_TOCHAR(glyph_atlas_size),sizeof(uint32));
        export_to_png(data);
        system_logger->info() << "Created new cached font data";
    }
    void load_locale_data(){
        std::ifstream file(".cache/font.bin",std::ios::binary);
        if(!file.is_open())throw std::runtime_error("Coudn't load cached font data!");
        unsigned count;
        file.read(F_TOCHAR(character_scale),sizeof(unsigned));
        file.read(F_TOCHAR(count),sizeof(unsigned));
        for(unsigned i = 0;i<count;i++){
            unsigned character;
            file.read(F_TOCHAR(character),sizeof(unsigned));

            PRIV::GlyphMetaData data;
            file.read(F_TOCHAR(data),sizeof(PRIV::GlyphMetaData));

            glyphs[character] = data;
        }
        uint32 atlasSize;
        file.read(F_TOCHAR(atlasSize),sizeof(atlasSize));
        file.close();

        int w,h;
        uint8* data = import_from_png(".cache/font_atlas.png", &w, &h);
        if (w != atlasSize || h != atlasSize) {
            stbi_image_free(data);
            throw std::runtime_error("Cached PNG size doesn't match metadata");
        }
        glyph_atlas_size = atlasSize;
        create_atlas(data);
        delete[] data;
    }
    bool check_locale_data(){
        return std::filesystem::exists(".cache/font.bin");
    }
}


namespace PRIV{
    namespace FontLoader{
        const LiteAPI::Texture *glyph_atlas_ptr = nullptr;
        const uint8 *character_size_ptr = nullptr;
        void loadfrom(std::string _path){
            fontloader_config = &(*LiteDATA::main_config)["fontloader"];
            font_file_format = (*fontloader_config)["font_format"];
            std::string full_path = _path+"."+font_file_format;
            if(check_locale_data()){
                load_locale_data();
            }else{
                load_glyphs(full_path);
            }
            glyph_atlas_ptr=glyph_atlas;
            character_size_ptr=&character_scale;
        }
        const GlyphMetaData& getGlyphMetaData(wchar_t character){
            auto it = glyphs.find(character);
            if(it==glyphs.end()){
                system_logger->error() << "Couldn't find glyph data! Check res/locale.loc. Glyph: " << (int)character;
                return glyphs['*'];
            }
            return it->second;
        }
        void close(){
            delete glyph_atlas;
        }
    }
}