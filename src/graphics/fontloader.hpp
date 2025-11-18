#pragma once
#include <string>
#include <LiteGL/system/types.hpp>

namespace LiteAPI{
    class Texture;
}

namespace PRIV{
    struct GlyphMetaData{
        vector2<float> texCoord;
        vector2<float> texSize;
        vector2<uint32> size;
        vector2<int> bearing;
        uint32 advance;
    };

    namespace FontLoader{
        extern const LiteAPI::Texture *glyph_atlas_ptr;
        extern const uint8 *character_size_ptr;
        void loadfrom(std::string _path);
        const GlyphMetaData& getGlyphMetaData(wchar_t character);
        void close();
    }
}