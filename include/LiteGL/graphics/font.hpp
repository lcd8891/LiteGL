#pragma once
#include <LiteGL/system/types.hpp>
#include <unordered_map>
#include <string>
#include <filesystem>

namespace LiteAPI{
    class Texture;
    class Font{
        public:
        struct GlyphMetaData{
            vector2<float> texCoord;
            vector2<float> texSize;
            vector2<uint32> size;
            vector2<int> bearing;
            uint32 advance;
        };
        private:
        LiteAPI::Texture* glyph_atlas;
        std::unordered_map<wchar_t,GlyphMetaData> glyphs;
        std::string font_name;
        
        void genFontData(std::filesystem::path path);
        void createAtlas(uint8 *data,const unsigned &glyph_atlas_size);
        void loadFontData();
        void saveFontData(uint8 *data,const unsigned &glyph_atlas_size);

        public:
        Font(std::filesystem::path path);
        ~Font();

        Texture* getTexture();
        const GlyphMetaData& operator[](wchar_t ch);

    };
}