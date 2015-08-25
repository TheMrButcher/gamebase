#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/TextBoxSkin.h>
#include <gamebase/engine/AnimatedObject.h>
#include <gamebase/engine/EditableLabel.h>
#include <gamebase/engine/TextBoxCursor.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API AnimatedTextBoxSkin : public TextBoxSkin, public AnimatedObject {
public:
    AnimatedTextBoxSkin(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeGeometry>& geom = nullptr);

    EditableLabel& label() { return m_text; }
    TextBoxCursor& cursor() { return m_cursor; }

    virtual void setText(const std::string& text) override
    {
        m_text.setText(text);
    }

    virtual void setSelection(size_t startIndex, size_t endIndex) override;

    virtual const std::vector<CharPosition>& textGeometry() const override
    {
        return m_text.textGeometry();
    }

    virtual std::shared_ptr<IRelativeGeometry> geometry() const override
    {
        return m_geom;
    }

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& s) const override;

private:
    size_t m_cursorPos;
    bool m_drawCursor;
    bool m_loaded;
    
    EditableLabel m_text;
    TextBoxCursor m_cursor;
};

}
