#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/LinearLayoutSkin.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/engine/AlignEnums.h>
#include <gamebase/engine/RelativeValue.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API HorizontalLayoutSkin : public LinearLayoutSkin, public ISerializable {
public:
    HorizontalLayoutSkin(const std::shared_ptr<IRelativeBox>& box)
        : m_box(box)
        , m_padding(RelType::Pixels, 0)
        , m_adjustSize(true)
        , m_align(VertAlign::Top)
    {}

    const RelativeValue& padding() const { return m_padding; }
    void setPadding(const RelativeValue& padding) { m_padding = padding; }

    bool adjustSize() const { return m_adjustSize; }
    void setAdjustSize(bool value) { m_adjustSize = value; }

    VertAlign::Enum align() const { return m_align; }
    void setAlign(VertAlign::Enum align) { m_align = align; }

    virtual BoundingBox listBox() const override
    {
        return m_box->get();
    }

    virtual Direction::Enum direction() const override
    {
        return Direction::Horizontal;
    }

    virtual std::shared_ptr<IRelativeOffset> createOffset(size_t index) const override;
    virtual void setExtent(const BoundingBox& box) override;

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
    RelativeValue m_padding;
    bool m_adjustSize;
    VertAlign::Enum m_align;
};

}
