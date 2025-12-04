#pragma once
#include <LiteGL/system/types.hpp>
#include <unordered_map>
#include <string>

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
        std::string font_file_format;
        uint32 glyph_atlas_size;
        uint8 character_scale;

        const Te
    };
}