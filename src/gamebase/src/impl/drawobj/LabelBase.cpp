/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/drawobj/LabelBase.h>
#include "src/impl/text/ITextRenderer.h"
#include <gamebase/impl/text/TextGeometry.h>
#include <gamebase/impl/graphics/TextureProgram.h>

namespace gamebase { namespace impl {
    
void LabelBase::setTextAndLoad(const std::string& text)
{
    m_text = text;
    if (m_rect.isValid())
        loadResources();
}

void LabelBase::setAlignProperties(const AlignProperties& alignProps)
{
    m_alignProps = alignProps;
    if (m_renderer) {
        if (m_font != m_alignProps.font.get()) {
            loadResources();
            return;
        }

        m_renderer->setUnderlined(alignProps.font.underlined);
        m_renderer->setLineThrough(alignProps.font.lineThrough);
    }
}

void LabelBase::setColor(const GLColor& color)
{
    m_color = color;
    if (m_renderer)
        m_renderer->setColor(color);
}

void LabelBase::setOutlineColor(const GLColor & color)
{
    m_outlineColor = color;
    if (m_renderer)
        m_renderer->setOutlineColor(color);
}

void LabelBase::loadResources()
{
    m_font = m_alignProps.font.get();
    try {
        auto alignedText = alignText(m_text, m_alignProps, m_rect);
        m_renderer = m_font->makeRenderer();
        m_renderer->load(alignedText);
        m_renderer->setColor(m_color);
        m_renderer->setOutlineColor(m_outlineColor);
        m_renderer->setUnderlined(m_alignProps.font.underlined);
        m_renderer->setLineThrough(m_alignProps.font.lineThrough);
    } catch (std::exception& ex) {
        std::cout << "Error while trying to load text \"" << m_text << "\" to Label"
            ". Reason: " << ex.what() << std::endl;
        return;
    }
}

void LabelBase::drawAt(const Transform2& position) const
{
    if (!m_renderer || m_renderer->empty())
        return;
    
    m_renderer->render(position);
}

void LabelBase::setBox(const BoundingBox& allowedBox)
{
    m_rect = allowedBox;
    if (m_adjustSize) {
        auto alignedText = alignText(m_text, m_alignProps, allowedBox);
        BoundingBox extent;
        for (auto it = alignedText.begin(); it != alignedText.end(); ++it)
            extent.add(it->bbox);
        if (extent.isValid())
            m_rect = extent;
        else
            m_rect.topRight = m_rect.bottomLeft;
    }
}

} }
