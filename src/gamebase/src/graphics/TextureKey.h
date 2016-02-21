#pragma once

#include <gamebase/graphics/Texture.h>

namespace gamebase {

struct TextureKey {
    TextureKey() {}

    TextureKey(const std::string& id)
        : id(id)
        , wrapX(Texture::Clamp)
        , wrapY(Texture::Clamp)
    {}

    TextureKey(const std::string& id, Texture::WrapMode wrapX, Texture::WrapMode wrapY)
        : id(id)
        , wrapX(wrapX)
        , wrapY(wrapY)
    {}

    std::string id;
    Texture::WrapMode wrapX;
    Texture::WrapMode wrapY;
};

inline bool operator==(const TextureKey& k1, const TextureKey& k2)
{
    return k1.wrapX == k2.wrapX
        && k1.wrapY == k2.wrapY
        && k1.id == k2.id;
}

struct TextureKeyHash {
    size_t operator()(const TextureKey& k) const
    {
        static const std::hash<std::string> STR_HASH;
        static const std::hash<int> INT_HASH;
        return STR_HASH(k.id)
            ^ (INT_HASH(static_cast<int>(k.wrapX) + (static_cast<int>(k.wrapY) << 4)) << 1);
    }
};

}
