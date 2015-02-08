#pragma once

#include <gamebase/engine/ITextFilter.h>
#include <gamebase/engine/IInputProcessor.h>
#include <gamebase/engine/TextEditSkin.h>
#include <gamebase/engine/FindableGeometry.h>
#include <gamebase/engine/OffsettedPosition.h>

namespace gamebase {

class GAMEBASE_API TextEdit : public OffsettedPosition, public FindableGeometry, public Selectable, public IInputProcessor, public IDrawable {
public:
    TextEdit(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<TextEditSkin>& skin,
        const std::shared_ptr<ITextFilter>& textFilter = nullptr);

    const std::string& text() const { return m_text; }

    void setText(const std::string& text);

    virtual void setSelectionState(SelectionState::Enum state) override;

    virtual void loadResources() override { m_skin->loadResources(); }

    virtual void draw(const Transform2& globalPosition) const
    {
        m_skin->draw(m_offset->get() * globalPosition);
    }

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_skin->setBox(allowedBox);
        m_offset->setBoxes(allowedBox, box());
    }

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual void processInput(const InputRegister& input) override;

private:
    void processKey(char key);
    void processSpecialKey(SpecialKey::Enum key);
    void processMouse(const InputRegister& input);
    void setCursor(size_t pos);
    void moveCursor(int shift);
    size_t calcCharIndex(float x);

    std::shared_ptr<TextEditSkin> m_skin;
    std::shared_ptr<ITextFilter> m_textFilter;
    std::string m_text;
    size_t m_selectionStart;
    size_t m_selectionEnd;
};

}
