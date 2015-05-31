#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/ScrollBarSkin.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/engine/ObjectsCollection.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API SimpleScrollBarSkin : public ScrollBarSkin, public ISerializable {
public:
    SimpleScrollBarSkin(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeBox>& dragBox,
        Direction::Enum direction)
        : m_box(box)
        , m_direction(direction)
    {}

    void addElement(const std::shared_ptr<IObject>& element)
    {
        m_skinElements.addObject(element);
    }

    void setAlwaysShow(bool value) { m_alwaysShow = value; }

    void setDecButtonSkin(
        const std::shared_ptr<ButtonSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setIncButtonSkin(
        const std::shared_ptr<ButtonSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setDragBarSkin(const std::shared_ptr<ButtonSkin>& skin)
    {
        m_dragBarSkin = skin;
    }

    virtual std::shared_ptr<Button> createDecButton() const override;
    virtual std::shared_ptr<Button> createIncButton() const override;

    virtual std::shared_ptr<ScrollDragBar> createDragBar(
        const std::shared_ptr<FixedOffset>& position) const override;

    virtual BoundingBox dragBox() const override { return m_dragBox->get(); }
    virtual Direction::Enum direction() const override { return m_direction; }
    virtual bool alwaysShow() const override { return m_alwaysShow; }

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
        m_dragBox->setParentBox(box());
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
    std::shared_ptr<IRelativeBox> m_dragBox;
    Direction::Enum m_direction;

    bool m_alwaysShow;
    std::shared_ptr<ButtonSkin> m_decButtonSkin;
    std::shared_ptr<IRelativeOffset> m_decButtonPosition;
    std::shared_ptr<ButtonSkin> m_incButtonSkin;
    std::shared_ptr<IRelativeOffset> m_incButtonPosition;
    std::shared_ptr<ButtonSkin> m_dragBarSkin;

    ObjectsCollection m_skinElements;
};

}
