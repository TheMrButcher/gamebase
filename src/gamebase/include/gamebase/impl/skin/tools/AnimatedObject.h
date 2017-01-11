/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/engine/Selectable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/tools/ObjectsCollection.h>
#include <gamebase/impl/anim/IAnimation.h>
#include <gamebase/impl/anim/AnimationManager.h>
#include <gamebase/impl/geom/IdenticGeometry.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

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

    virtual void setSelectionState(SelectionState::Enum state) override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override
    {
        m_skinElements.draw(position);
    }
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        builder->registerObject(&m_skinElements);
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

} }
