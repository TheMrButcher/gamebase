/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/pos/IPositionable.h>
#include <gamebase/impl/text/AlignProperties.h>
#include <gamebase/impl/graphics/GLBuffers.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/impl/geom/BoundingBox.h>

namespace gamebase { namespace impl {

class GAMEBASE_API LabelBase : public Drawable {
public:
    LabelBase(const IPositionable* position = nullptr)
        : Drawable(position)
        , m_adjustSize(true)
    {}

    const std::string& text() const { return m_text; }
    virtual void setText(const std::string& text) { m_text = text; }
    void setTextAndLoad(const std::string& text);

    const AlignProperties& alignProperties() const { return m_alignProps; }
    void setAlignProperties(const AlignProperties& alignProps) { m_alignProps = alignProps; }

    const Color& color() const { return m_color; }
    void setColor(const Color& color) { m_color = color; }

    bool adjustSize() const { return m_adjustSize; }
    void setAdjustSize(bool value) { m_adjustSize = value; }

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override { return m_rect; }

protected:
    GLBuffers m_buffers;
    std::shared_ptr<IFont> m_font;

    BoundingBox m_rect;
    std::string m_text;
    AlignProperties m_alignProps;
    Color m_color;
    bool m_adjustSize;
};

} }
