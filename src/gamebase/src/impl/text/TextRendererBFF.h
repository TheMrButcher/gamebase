/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "ITextRenderer.h"
#include <gamebase/impl/graphics/GLTexture.h>

namespace gamebase { namespace impl {

class IFont;

class TextRendererBFF : public ITextRenderer {
public:
    TextRendererBFF(const IFont* font, const GLTexture& texture);

    virtual void load(const std::vector<AlignedString>& alignedText) override;

    virtual void setColor(const GLColor& color) override { m_color = color; }
    virtual void setOutlineColor(const GLColor&) override {}
    virtual void setUnderlined(bool) override {}
    virtual void setLineThrough(bool) override {}

    virtual bool empty() const override;

    virtual void render(const Transform2& pos) override;

private:
    GLBuffers m_buffers;
    const IFont* m_font;
    GLTexture m_texture;
    GLColor m_color;
};

} }
