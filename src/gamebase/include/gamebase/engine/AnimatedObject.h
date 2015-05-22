#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/Selectable.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/engine/ObjectsCollection.h>
#include <gamebase/engine/IAnimation.h>
#include <gamebase/engine/AnimationManager.h>
#include <gamebase/geom/IdenticGeometry.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API AnimatedObject : public virtual Drawable, public virtual Selectable,
    public virtual Registrable, public ISerializable {
public:
    AnimatedObject(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeGeometry>& geom)
        : m_box(box)
        , m_geom(geom ? geom : std::make_shared<IdenticGeometry>())
    {}

    void addElement(const std::shared_ptr<IObject>& element)
    {
        m_skinElements.addObject(element);
    }

    void setTransitAnimation(
        SelectionState::Enum state, const std::shared_ptr<IAnimation>& anim)
    {
        m_animations[state] = anim;
    }

    virtual void setSelectionState(SelectionState::Enum state) override
    {
        m_selectionState = state;
        m_animManager.reset();
        auto it = m_animations.find(state);
        if (it != m_animations.end())
            m_animManager.addAnimation(it->second);
    }

    virtual void loadResources() override
    {
        m_skinElements.loadResources();
        for (auto it = m_animations.begin(); it != m_animations.end(); ++it)
            it->second->load(m_register);
    }

    virtual void drawAt(const Transform2& position) const override
    {
        m_animManager.step();
        m_skinElements.draw(position);
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        m_skinElements.setBox(m_box->get());
        m_geom->setBox(m_box->get());
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        builder->registerObject("elements", &m_skinElements);
    }

    virtual void serialize(Serializer& s) const override;

protected:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IRelativeGeometry> m_geom;

    ObjectsCollection m_skinElements;
    std::map<SelectionState::Enum, std::shared_ptr<IAnimation>> m_animations;
    AnimationManager m_animManager;
};

class Deserializer;
GAMEBASE_API void deserializeAnimatedObjectElements(
    Deserializer& deserializer, AnimatedObject* obj);

}
