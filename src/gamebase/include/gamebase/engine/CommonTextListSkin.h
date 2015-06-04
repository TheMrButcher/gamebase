#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/TextListSkin.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/engine/ObjectsCollection.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API CommonTextListSkin : public TextListSkin, public ISerializable {
public:
    CommonTextListSkin(const std::shared_ptr<IRelativeBox>& box);

    void addElement(const std::shared_ptr<IObject>& element)
    {
        m_skinElements.addObject(element);
    }

    void setTextEditDisabled(bool value) { m_textEditDisabled = value; }
    bool textEditDisabled() const { return m_textEditDisabled; }

    void setOpenButtonSkin(
        const std::shared_ptr<ButtonSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setTextEditSkin(
        const std::shared_ptr<TextEditSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setButtonListSkin(
        const std::shared_ptr<ButtonListSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    virtual std::shared_ptr<PressableButton> createOpenButton() const override;
    virtual std::shared_ptr<TextEdit> createTextEdit() const override;
    virtual std::shared_ptr<ButtonList> createList() const override;

    virtual void loadResources() override
    {
        m_skinElements.loadResources();
    }

    virtual void drawAt(const Transform2& position) const override
    {
        m_skinElements.draw(position);
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        m_skinElements.setBox(box());
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;

    virtual void serialize(Serializer& s) const override;

protected:
    std::shared_ptr<IRelativeBox> m_box;

    bool m_textEditDisabled;

    std::shared_ptr<ButtonSkin> m_openButtonSkin;
    std::shared_ptr<IRelativeOffset> m_openButtonPosition;
    std::shared_ptr<TextEditSkin> m_textEditSkin;
    std::shared_ptr<IRelativeOffset> m_textEditPosition;
    std::shared_ptr<ButtonListSkin> m_buttonListSkin;
    std::shared_ptr<IRelativeOffset> m_buttonListPosition;

    ObjectsCollection m_skinElements;
};

}
