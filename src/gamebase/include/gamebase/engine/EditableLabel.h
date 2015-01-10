#pragma once

#include <gamebase/engine/FilledRect.h>
#include <gamebase/text/TextGeometry.h>
#include <algorithm>

namespace gamebase {

class GAMEBASE_API EditableLabel : public IDrawable {
public:
    EditableLabel();

    void setText(const std::string& text);
    void setAlignProperties(const AlignProperties& alignProps) { m_alignProps = alignProps; }
    void setColor(const Color& color) { m_color = color; }
    void setSelection(size_t startIndex, size_t endIndex)
    {
        m_selection = std::minmax(startIndex, endIndex);
    }

    const std::vector<CharPosition>& textGeometry() const { return m_textGeom; }

    virtual void loadResources() override;
    virtual void draw(const Transform2& globalPosition) const override;

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_rect = allowedBox;
    }

    virtual BoundingBox box() const override { return m_rect; }

private:
    void updateTextGeometry();

    FilledRect m_selectionRect;
    GLBuffers m_buffers;
    std::shared_ptr<IFont> m_font;
    std::vector<CharPosition> m_textGeom;

    BoundingBox m_rect;
    std::string m_text;

    AlignProperties m_alignProps;
    Color m_color;
    Color m_selectedTextColor;
    std::pair<size_t, size_t> m_selection;
};

}
