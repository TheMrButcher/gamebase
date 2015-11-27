#pragma once

#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/PositionElement.h>
#include <gamebase/engine/Identifiable.h>

namespace gamebase {

class InactiveObjectConstruct : public IPositionable, public Drawable,
    public ISerializable, public Registrable, public Identifiable {
public:
    InactiveObjectConstruct(
        const std::shared_ptr<IDrawable>& drawable,
        const std::shared_ptr<PositionElement>& position = nullptr);

    Vec2 getOffset() const { return m_posElement->getOffset(); }
    void setOffset(const Vec2& v) { m_posElement->setOffset(v); }

    float scale() const { return m_posElement->scale(); }
    void setScale(float scale) { m_posElement->setScale(scale); }

    float scaleX() const { return m_posElement->scaleX(); }
    void setScaleX(float scale) { m_posElement->setScaleX(scale); }

    float scaleY() const { return m_posElement->scaleY(); }
    void setScaleY(float scale) { m_posElement->setScaleY(scale); }

    float angle() const { return m_posElement->angle(); }
    void setAngle(float angle) { m_posElement->setAngle(angle); }

    virtual Transform2 position() const override { return m_posElement->position(); }

    virtual void loadResources() override
    {
        m_drawable->loadResources();
    }

    virtual void drawAt(const Transform2& position) const
    {
        m_drawable->draw(position);
    }

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_drawable->setBox(allowedBox);
    }

    virtual BoundingBox box() const override
    {
        return m_drawable->box();
    }

    virtual void registerObject(PropertiesRegisterBuilder*) override;
    virtual void serialize(Serializer& serializer) const override;

private:
    std::shared_ptr<IDrawable> m_drawable;
    std::shared_ptr<PositionElement> m_posElement;
};

}
