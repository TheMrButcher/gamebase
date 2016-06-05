#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/base/TextBoxSkin.h>
#include <gamebase/impl/skin/tools/AnimatedObject.h>
#include <gamebase/impl/skin/tools/EditableLabel.h>
#include <gamebase/impl/skin/tools/TextBoxCursor.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API AnimatedTextBoxSkin : public TextBoxSkin, public AnimatedObject {
public:
    AnimatedTextBoxSkin(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeGeometry>& geom = nullptr);

    AnimatedTextBoxSkin(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<EditableLabel>& text,
        const std::shared_ptr<TextBoxCursor>& cursor,
        const std::shared_ptr<IRelativeGeometry>& geom = nullptr);

    void setShiftPeriod(Time value);

    virtual void setText(const std::string& text) override
    {
        m_text->setText(text);
    }

    virtual void setSelection(size_t startIndex, size_t endIndex) override;

    virtual const std::vector<CharPosition>& textGeometry() const override
    {
        return m_text->textGeometry();
    }

    virtual std::shared_ptr<IRelativeGeometry> geometry() const override
    {
        return m_geom;
    }

    virtual Time shiftPeriod() const
    {
        return m_shiftPeriod;
    }

    virtual void setOffsetX(float x)
    {
        m_cursorOffsetX = x;
        m_text->setOffsetX(x);
    }
    
    virtual BoundingBox textAreaBox() const override
    {
        return m_text->box();
    }

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& s) const override;

private:
    Time m_shiftPeriod;

    float m_cursorOffsetX;
    size_t m_cursorPos;
    bool m_drawCursor;
    bool m_loaded;
    
    std::shared_ptr<EditableLabel> m_text;
    std::shared_ptr<TextBoxCursor> m_cursor;
};

} }
