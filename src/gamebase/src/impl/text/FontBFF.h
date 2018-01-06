/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/text/IFont.h>

namespace gamebase { namespace impl {

class FontMetaData;

class FontBFF : public IFont {
public:
    FontBFF(const std::string& fontFileName, const std::string& metadataFileName);

    const std::string& fileName() const { return m_fileName; }
    bool isLoaded() const;
    void load();

    virtual const std::string& familyName() const override { return m_name; }

    virtual float fontSize() const override { return static_cast<float>(m_fontSize); }

    virtual float capHeight() const override { return fontSize(); }

    virtual float lineSpacing() const override { return fontSize(); }

    virtual const GLTexture& texture() const override { return m_glyphAtlas; }

    virtual std::vector<uint32_t> glyphIndices(const std::string& utfStr) const override;

    virtual float advance(uint32_t glyphIndex) const override
    {
        if (glyphIndex >= m_glyphsNum)
            return 0.f;
        glyphIndex += m_minIndex;
        return static_cast<float>(m_glyphWidths[glyphIndex]);
    }

    virtual float kerning(uint32_t glyphIndex1, uint32_t glyphIndex2) const
    {
        return 0;
    }

    virtual BoundingBox bounds(uint32_t glyphIndex) const override
    {
        return BoundingBox(Vec2(0, 0), m_cellSize);
    }

    virtual BoundingBox glyphTextureRect(uint32_t glyphIndex) const override;

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

} }
