#pragma once

#include <gamebase/engine/ObjectsCollection.h>
#include <gamebase/engine/ProjectionTransform.h>

namespace gamebase {

class ObjectReflection : public IPositionable, public IFindable, public IDrawable {
public:
    ObjectReflection(
        IPositionable* positionable,
        IDrawable* drawable)
        : m_positionable(positionable)
        , m_drawable(drawable)
    {}

    void addObject(const std::shared_ptr<IObject>& object)
    {
        m_children.addObject(object);
        if (auto* positionable = dynamic_cast<IPositionable*>(object.get()))
            positionable->setParentPosition(m_positionable);
    }

    virtual Transform2 position() const override { return m_positionable->fullTransform(); }
    virtual Transform2 fullTransform() const override { return position(); }
    virtual void setParentPosition(const IPositionable*) {}
    
    virtual IObject* find(
        const Vec2& point, const Transform2&) override
    {
        if (!isVisible())
            return nullptr;
        return m_children.find(point, position());
    }

    virtual void loadResources() override { m_children.loadResources(); }

    virtual void draw(const Transform2&) const override
    {
        if (isVisible())
            drawAt(m_positionable->fullTransform() * projectionTransform());
    }

    virtual void drawAt(const Transform2& position) const override { m_children.draw(position); }

    virtual void setBox(const BoundingBox&)
    {
        m_children.setBox(m_drawable->box());
    }

    virtual BoundingBox box() const { return m_drawable->box(); }

    virtual void setVisible(bool visible) override { m_drawable->setVisible(visible); }

    virtual bool isVisible() const override { return m_drawable->isVisible(); }

private:
    IPositionable* m_positionable;
    IDrawable* m_drawable;
    ObjectsCollection m_children;
};

}
