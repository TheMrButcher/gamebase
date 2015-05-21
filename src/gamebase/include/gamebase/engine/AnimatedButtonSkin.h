#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/ButtonSkin.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/engine/ObjectsCollection.h>
#include <gamebase/engine/IAnimation.h>
#include <gamebase/engine/AnimationManager.h>
#include <gamebase/serial/ISerializable.h>
#include <gamebase/geom/IRelativeGeometry.h>

namespace gamebase {

class GAMEBASE_API AnimatedButtonSkin : public ButtonSkin, public ISerializable {
public:
    AnimatedButtonSkin(
        const std::shared_ptr<IRelativeBox>& box);
    
    AnimatedButtonSkin(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeGeometry>& geom)
        : m_box(box)
        , m_geom(geom)
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

    virtual std::shared_ptr<IRelativeGeometry> geometry() const override
    {
        return m_geom;
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        builder->registerObject("elements", &m_skinElements);
    }

    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IRelativeGeometry> m_geom;

    ObjectsCollection m_skinElements;
    std::map<SelectionState::Enum, std::shared_ptr<IAnimation>> m_animations;
    AnimationManager m_animManager;
};

}
