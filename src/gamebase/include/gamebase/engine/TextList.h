#pragma once

#include <gamebase/engine/TextListSkin.h>

namespace gamebase {

class GAMEBASE_API TextList : public OffsettedPosition, public IFindable,
    public Drawable, public Registrable, public ISerializable {
public:
    TextList(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<TextListSkin>& skin);

    const std::string& text() const { return m_textEdit->text(); }
    void setText(const std::string& text) { m_textEdit->setText(text); }

    int currentVariantID() const;

    void setCallback(const std::function<void(const std::string&, int)>& callback)
    {
        m_callback = callback;
    }

    void addButton(const std::string& text, const std::shared_ptr<Button>& button);
    void addButton(const std::string& text, const std::shared_ptr<Button>& button, unsigned int id);

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override;

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    void changeState(bool isOpened);
    void setTextFromVariant(int id);

    std::shared_ptr<TextListSkin> m_skin;
    std::shared_ptr<PressableButton> m_openButton;
    std::shared_ptr<TextEdit> m_textEdit;
    std::function<void(const std::string&, int)> m_callback;
    std::vector<std::string> m_textVariants;
    std::vector<int> m_textIDs;
    int m_nextID;
    std::shared_ptr<ButtonList> m_list;
    
    bool m_isListOpened;
};

}
