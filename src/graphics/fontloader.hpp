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
        extern LiteAPI::Texture *glyph_atlas_ptr;
        void loadfrom(std::string _path);
        const GlyphMetaData& getGlyphMetaData(wchar_t character);
        void close();
    }
}