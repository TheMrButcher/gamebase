#pragma once

#include <gamebase/text/IFont.h>

namespace gamebase {

class FontMetaData;

class Font : public IFont {
public:
    Font(const std::string& fontFileName, const std::string& metadataFileName);

    void load();

    virtual const std::string& familyName() const override { return m_name; }

    virtual float fontSize() const override { return static_cast<float>(m_fontSize); }

    virtual Vec2 cellSize() const override { return m_cellSize; }

    virtual const GLTexture& texture() const override { return m_glyphAtlas; }

    virtual std::vector<size_t> glyphIndices(const std::string& utfStr) const override;

    virtual float getWidth(size_t glyphIndex) const override
    {
        if (glyphIndex >= m_glyphsNum)
            return 0.f;
        glyphIndex += m_minIndex;
        return static_cast<float>(m_glyphWidths[glyphIndex]);
    }

    virtual BoundingBox glyphTextureRect(size_t glyphIndex) const override;

private:
    std::string m_name;
    std::string m_fileName;
    std::string m_metaDataFileName;
    unsigned int m_fontSize;
    unsigned char m_minIndex;
    size_t m_glyphsNum;
    unsigned char m_glyphsPerLine;
    Vec2 m_cellSize;
    Vec2 m_glyphTextureSize;
    unsigned char m_glyphWidths[256];
    GLTexture m_glyphAtlas;
    std::shared_ptr<FontMetaData> m_metaData;
};

}
