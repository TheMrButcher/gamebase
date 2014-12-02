#pragma once

#include <gamebase/text/IFont.h>

namespace gamebase {

class Font : public IFont {
public:
    Font(const std::string& fileName);

    void load();

    virtual const std::string& familyName() const override { return m_name; }

    virtual float fontSize() const override { return static_cast<float>(m_fontSize); }

    virtual Vec2 cellSize() const override { return m_cellSize; }

    virtual const Texture& texture() const override { return m_glyphAtlas; }

    virtual float getWidth(char ch) const override
    {
        return static_cast<float>(m_glyphWidths[static_cast<unsigned char>(ch)]);
    }

    virtual BoundingBox glyphTextureRect(char ch) const override;

private:
    std::string m_name;
    std::string m_fileName;
    unsigned int m_fontSize;
    unsigned char m_minIndex;
    unsigned char m_maxIndex;
    unsigned char m_glyphsPerLine;
    Vec2 m_cellSize;
    Vec2 m_glyphTextureSize;
    unsigned char m_glyphWidths[256];
    Texture m_glyphAtlas;
};

}
