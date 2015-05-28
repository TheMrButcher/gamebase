#pragma once

#include <gamebase/serial/ISerializable.h>
#include <string>
#include <unordered_map>
#include <stdint.h>

namespace gamebase {

class FontMetaData : public ISerializable {
public:
    FontMetaData();

    FontMetaData(char firstGlyph, char lastGlyph);

    std::vector<size_t> glyphIndices(const std::string& utf8Str) const;
        
    virtual void serialize(Serializer& s) const override;

private:
    friend std::unique_ptr<IObject> deserializeFontMetaData(Deserializer&);

    std::unordered_map<uint64_t, size_t> m_glyphIndices;
};

}
