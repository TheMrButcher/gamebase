#pragma once

#include <gamebase/engine/ITextFilter.h>
#include <gamebase/engine/IInputProcessor.h>
#include <gamebase/engine/TextBoxSkin.h>
#include <gamebase/engine/FindableGeometry.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/text/Utf8Text.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API TextBox : public OffsettedPosition, public FindableGeometry,
    public Selectable, public IInputProcessor, public Drawable, public Registrable, public ISerializable {
public:
    TextBox(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<TextBoxSkin>& skin,
        const std::shared_ptr<ITextFilter>& textFilter = nullptr);

    const std::string& text() const { return m_text.toString(); }

    void setText(const std::string& text);

    void setCallback(const std::function<void(std::string)>& callback)
    {
        m_callback = callback;
    }

    virtual void setSelectionState(SelectionState::Enum state) override;

    virtual void loadResources() override
    {
        m_inited = true;
        m_skin->loadResources();
    }

    virtual void drawAt(const Transform2& position) const
    {
        m_skin->draw(position);
    }

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_skin->setBox(allowedBox);
        setPositionBoxes(allowedBox, box());
    }

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual void processInput(const InputRegister& input) override;

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    void processKey(char key);
    void processSpecialKey(SpecialKey::Enum key);
    void processMouse(const InputRegister& input);
    void setCursor(size_t pos);
    void moveCursor(int shift);
    size_t calcCharIndex(float x);

    std::shared_ptr<TextBoxSkin> m_skin;
    std::shared_ptr<ITextFilter> m_textFilter;
    Utf8Text m_text;
    size_t m_selectionStart;
    size_t m_selectionEnd;
    bool m_inited;
    std::function<void(std::string)> m_callback;
};

}
