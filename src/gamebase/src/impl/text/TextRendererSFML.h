/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "ITextRenderer.h"
#include <SFML/Graphics/Text.hpp>

namespace gamebase { namespace impl {

class FontSFML;

class TextRendererSFML : public ITextRenderer {
public:
    TextRendererSFML(const FontSFML* font);

    virtual void load(const std::vector<CharPosition>& textGeom) override;

    virtual void setColor(const GLColor& color) override;
    virtual void setOutlineColor(const GLColor& color) override;
    virtual void setUnderlined(bool enabled) override;
    virtual void setLineThrough(bool enabled) override;

    virtual bool empty() const override;

    virtual void render(const Transform2& pos) override;

private:
    const FontSFML* m_font;
    sf::Text m_renderedText;
};

} }
