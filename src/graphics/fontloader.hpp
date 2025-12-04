#pragma once
#include <string>

namespace LiteAPI{
    class Texture;
}

namespace PRIV{

    namespace FontLoader{
        extern const LiteAPI::Texture *glyph_atlas_ptr;
        extern const uint8 *character_size_ptr;
        void loadfrom(std::string _path);
        const GlyphMetaData& getGlyphMetaData(wchar_t character);
        void close();
    }
}