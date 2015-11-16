#pragma once

#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/IPositionable.h>
#include <gamebase/text/AlignProperties.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/graphics/GLBuffers.h>
#include <gamebase/geom/BoundingBox.h>

namespace gamebase {

class GAMEBASE_API Label : public Drawable {
public:
    Label(const IPositionable* position = nullptr)
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

}
