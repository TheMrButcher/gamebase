#pragma once

#include <gamebase/engine/FilledRect.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/text/TextGeometry.h>
#include <gamebase/serial/ISerializable.h>
#include <algorithm>

namespace gamebase {

class GAMEBASE_API EditableLabel : public OffsettedPosition, public Drawable,
    public Registrable, public ISerializable {
public:
    EditableLabel(const std::shared_ptr<IRelativeBox>& box);

    void setRelativeBox(const std::shared_ptr<IRelativeBox>& box) { m_box = box; }
    void setText(const std::string& text);
    void setFont(const FontDesc& fontDesc) { m_alignProps.font = fontDesc; }
    void setColor(const Color& color) { m_color = color; }
    void setSelectionColor(const Color& color) { m_selectionRect.setColor(color); }
    void setSelection(size_t startIndex, size_t endIndex)
    {
        m_selection = std::minmax(startIndex, endIndex);
    }

    const std::vector<CharPosition>& textGeometry() const { return m_textGeom; }

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
    }

    virtual BoundingBox box() const override { return m_box->get(); }
    
    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        builder->registerColor("color", &m_color);
    }
    
    virtual void serialize(Serializer& s) const override;

private:
    void updateTextGeometry();

    std::shared_ptr<IRelativeBox> m_box;

    FilledRect m_selectionRect;
    GLBuffers m_buffers;
    std::shared_ptr<IFont> m_font;
    std::vector<CharPosition> m_textGeom;
    
    std::string m_text;
    AlignProperties m_alignProps;
    Color m_color;
    std::pair<size_t, size_t> m_selection;
};

}
