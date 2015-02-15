#pragma once

#include <gamebase/engine/TextListSkin.h>

namespace gamebase {

class GAMEBASE_API TextList : public OffsettedPosition, public IFindable, public Drawable {
public:
    TextList(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<TextListSkin>& skin);

    const std::string& text() const { return m_textEdit->text(); }

    void setText(const std::string& text) { m_textEdit->setText(text); }

    void addButton(const std::string& text, const std::shared_ptr<Button>& button);

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override;

private:
    void changeState(bool isOpened);
    void setTextFromVariant(const std::string& text);

    std::shared_ptr<TextListSkin> m_skin;
    std::shared_ptr<PressableButton> m_openButton;
    std::shared_ptr<TextEdit> m_textEdit;
    std::shared_ptr<ButtonList> m_list;
    
    bool m_isListOpened;
};

}
