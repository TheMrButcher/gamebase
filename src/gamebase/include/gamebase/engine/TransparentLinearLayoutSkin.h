#pragma once

#include <gamebase/engine/LinearLayoutSkin.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class TransparentLinearLayoutSkin : public LinearLayoutSkin, public ISerializable {
public:
    TransparentLinearLayoutSkin(
        const std::shared_ptr<IRelativeBox>& box,
        Direction::Enum direction)
        : m_box(box)
        , m_direction(direction)
        , m_padding(0)
        , m_adjustSize(true)
    {}

    float padding() const { return m_padding; }
    void setPadding(float padding) { m_padding = padding; }

    bool adjustSize() const { return m_adjustSize; }
    void setAdjustSize(bool value) { m_adjustSize = value; }

    virtual BoundingBox listBox() const override
    {
        return m_box->get();
    }

    virtual Direction::Enum direction() const override
    {
        return m_direction;
    }

    virtual std::shared_ptr<IRelativeOffset> createOffset(size_t index) const override;
    virtual void setSize(float width, float height) override;

    virtual void loadResources() override {}
    virtual void drawAt(const Transform2& position) const override {}
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        m_curBox = m_box->get();
    }
    virtual BoundingBox box() const override
    {
        return m_curBox;
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override {}
    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IRelativeBox> m_box;
    BoundingBox m_curBox;
    Direction::Enum m_direction;
    float m_padding;
    bool m_adjustSize;
};

}
