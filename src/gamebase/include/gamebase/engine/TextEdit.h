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
        const Vec2& position,
        const std::shared_ptr<IGeometry>& geom,
        const std::shared_ptr<TextEditSkin>& skin);

    TextEdit(
        const Vec2& position,
        const std::shared_ptr<IGeometry>& geom,
        const std::shared_ptr<TextEditSkin>& skin,
        const std::shared_ptr<ITextFilter>& textFilter);

    const std::string& text() const { return m_text; }

    virtual void setSelectionState(SelectionState::Enum state) override;

    virtual void loadResources() override { m_skin->loadResources(); }

    virtual void draw(const Transform2& globalPosition) const
    {
        m_skin->draw(m_pos * globalPosition);
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
