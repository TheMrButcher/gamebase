/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/tools/EditableLabel.h>
#include "src/impl/text/ITextRenderer.h"
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

namespace {
bool containsXRange(const BoundingBox& b1, const BoundingBox& b2)
{
	return b2.bottomLeft.x >= b1.bottomLeft.x && b2.topRight.x <= b1.topRight.x; 
}
}

EditableLabel::EditableLabel(const std::shared_ptr<IRelativeBox>& box)
    : Drawable(this)
    , m_box(box)
    , m_offsetX(0)
    , m_isLimited(false)
{
    m_alignProps.horAlign = HorAlign::Left;
    m_alignProps.vertAlign = VertAlign::Center;
    m_alignProps.enableStacking = false;

    m_selectionRect.setColor(GLColor(0.8f, 0.8f, 1.0f));
}

std::pair<float, float> EditableLabel::yRange() const
{
    if (m_alignedText.empty())
        return std::make_pair(0.f, 0.f);
    return std::make_pair(m_alignedText[0].bbox.bottom(), m_alignedText[0].bbox.top());
}

void EditableLabel::setText(const std::string& text)
{
    if (m_text == text)
        return;
    m_text = text;
    if (m_font)
        updateTextGeometry();
    else
        m_textGeom.clear();
}

void EditableLabel::setColor(const GLColor & color)
{
    m_color = color;
    if (m_renderer)
        m_renderer->setColor(color);
}

void EditableLabel::setOutlineColor(const GLColor & color)
{
    m_outlineColor = color;
    if (m_renderer)
        m_renderer->setOutlineColor(color);
}

void EditableLabel::loadResources()
{
    m_font = m_alignProps.font.get();
    updateTextGeometry();

    Vec2 textOffset(m_offsetX, 0);
    size_t firstVisible = size_t(-1);
    size_t lastVisible = 0;
    m_visibleTextGeom.clear();
    auto labelBox = m_box->get();
    for (size_t i = 0; i + 1 != m_textGeom.size(); ++i) {
        auto charBox = m_textGeom[i].position;
        charBox.topRight.x = m_textGeom[i + 1].position.bottomLeft.x;
        charBox.move(textOffset);
		if (containsXRange(labelBox, charBox)) {
            if (i < firstVisible)
                firstVisible = i;
            m_visibleTextGeom.push_back(m_textGeom[i]);
            m_visibleTextGeom.back().position.move(textOffset);
            lastVisible = i;
        } else {
            if (!m_visibleTextGeom.empty())
                break;
        }
    }
    if (m_selection.first < m_selection.second && !m_alignedText.empty()) {
        size_t curStart = firstVisible > m_selection.first
            ? firstVisible : m_selection.first;
        size_t curEnd = lastVisible + 1 < m_selection.second
            ? lastVisible + 1 : m_selection.second;
        BoundingBox box(
            Vec2(
                m_textGeom.at(curStart).position.bottomLeft.x,
                m_alignedText[0].bbox.bottom()),
            Vec2(
                m_textGeom.at(curEnd).position.bottomLeft.x,
                m_alignedText[0].bbox.top()));
        box.move(textOffset);
        m_selectionRect.setBox(box);
        m_selectionRect.loadResources();
    }

    std::vector<AlignedString> alignedText;
    if (!m_visibleTextGeom.empty() && !m_alignedText.empty()) {
        BoundingBox box(
            Vec2(m_visibleTextGeom.front().position.left(), m_alignedText[0].bbox.bottom()),
            Vec2(m_visibleTextGeom.back().position.right(), m_alignedText[0].bbox.top()));
        std::vector<uint32_t> glyphs;
        for (auto ch : m_visibleTextGeom)
            glyphs.push_back(ch.glyphIndex);
        alignedText.emplace_back(box, std::move(glyphs));
    }

    try {
        m_renderer = m_font->makeRenderer();
        m_renderer->load(alignedText);
        m_renderer->setColor(m_color);
        m_renderer->setOutlineColor(m_outlineColor);
        m_renderer->setUnderlined(m_alignProps.font.underlined);
        m_renderer->setLineThrough(m_alignProps.font.lineThrough);
    }
    catch (std::exception& ex) {
        std::cout << "Error while trying to load text \"" << m_text << "\" to EditableLabel"
            ". Reason: " << ex.what() << std::endl;
        return;
    }
}

void EditableLabel::drawAt(const Transform2& position) const
{
	if (!m_renderer || m_renderer->empty())
        return;

    if (m_selection.first < m_selection.second)
        m_selectionRect.draw(position);

    m_renderer->render(position);
}

void EditableLabel::serialize(Serializer& s) const
{
    s << "color" << m_color << "outlineColor" << m_outlineColor
        << "selectionColor" << m_selectionRect.color()
		<< "isLimited" << m_isLimited << "font" << m_alignProps.font
		<< "box" << m_box;
}

std::unique_ptr<IObject> deserializeEditableLabel(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(GLColor, color);
    GLColor outlineColor;
    if (deserializer.hasMember("outlineColor"))
        deserializer >> "outlineColor" >> outlineColor;
    DESERIALIZE(FontDesc, font);
    DESERIALIZE(GLColor, selectionColor);
    DESERIALIZE_OPT(bool, isLimited, false);
    std::unique_ptr<EditableLabel> result(new EditableLabel(box));
    result->setColor(color);
    result->setOutlineColor(outlineColor);
    result->setFont(font);
    result->setSelectionColor(selectionColor);
    result->setLimited(isLimited);
    return std::move(result);
}

REGISTER_CLASS(EditableLabel);

void EditableLabel::updateTextGeometry()
{
    auto rect = m_box->get();
    m_alignedText = alignText(m_text + ' ', m_alignProps, rect);
    m_textGeom = createTextGeometry(m_alignedText, m_font.get());
    auto lastCharPosition = m_textGeom.back();
    bool removeLast = false;
    float frontX = m_textGeom.front().position.bottomLeft.x;
    if (!m_isLimited)
        return;
    while (!m_textGeom.empty()
        && m_textGeom.back().position.bottomLeft.x > rect.topRight.x) {
        removeLast = true;
        m_textGeom.pop_back();
        m_text.pop_back();
    }
    if (removeLast) {
        float x = m_textGeom.empty()
            ? frontX
            : m_textGeom.back().position.bottomLeft.x;
        float w = lastCharPosition.position.width();
        lastCharPosition.position.bottomLeft.x = x;
        lastCharPosition.position.topRight.x = x + w;
        if (m_textGeom.empty()) {
            m_textGeom.push_back(lastCharPosition);
        } else {
            m_textGeom.back() = lastCharPosition;
            m_text.pop_back();
        }
    }
}

} }
