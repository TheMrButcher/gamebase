#pragma once

#include <gamebase/engine/IDrawable.h>
#include <gamebase/engine/IPositionable.h>
#include <gamebase/text/AlignProperties.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/graphics/GLBuffers.h>
#include <gamebase/geom/BoundingBox.h>

namespace gamebase {

class GAMEBASE_API StaticLabel : public IDrawable {
public:
    StaticLabel(const IPositionable* position = nullptr)
        : m_drawPosition(position)
    {}

    void setRect(const BoundingBox& rect) { m_rect = rect; }
    void setText(const std::string& text) { m_text = text; }
    void setAlignProperties(const AlignProperties& alignProps) { m_alignProps = alignProps; }
    void setColor(const Color& color) { m_color = color; }

    virtual void loadResources() override;
    virtual void draw(const Transform2& globalPosition) const override;

private:
    const IPositionable* m_drawPosition;
    GLBuffers m_buffers;
    std::shared_ptr<IFont> m_font;

    BoundingBox m_rect;
    std::string m_text;
    AlignProperties m_alignProps;
    Color m_color;
};

}
