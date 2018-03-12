/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/tools/AnimatedObject.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

void AnimatedObject::setSelectionState(SelectionState::Enum state)
{
    m_selectionState = state;
    m_animManager.resetChannel(0);
    auto it = m_animations.find(state);
    if (it != m_animations.end()) {
        m_animManager.addAnimation(it->second, 0);
    } else {
        it = m_animations.find(SelectionState::None);
        if (it != m_animations.end())
            m_animManager.addAnimation(it->second, 0);
    }
}

void AnimatedObject::loadResources()
{
    m_skinElements.loadResources();
    for (auto it = m_animations.begin(); it != m_animations.end(); ++it)
        it->second->load(m_register);
    m_animManager.start();
}
    
void AnimatedObject::setBox(const BoundingBox& allowedBox)
{
    m_box->setParentBox(allowedBox);
    m_skinElements.setBox(m_box->get());
    m_geom->setBox(m_box->get());
}

void AnimatedObject::serialize(Serializer& s) const
{
    s << "box" << m_box << "geometry" << m_geom
        << "animations" << m_animations << "elements" << m_skinElements.objects();
}

void deserializeAnimatedObjectElements(
    Deserializer& deserializer, AnimatedObject* obj)
{
    typedef std::map<SelectionState::Enum, std::shared_ptr<IAnimation>> Animations;
    DESERIALIZE(Animations, animations);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, elements);
    for (auto it = animations.begin(); it != animations.end(); ++it)
        obj->setTransitAnimation(it->first, it->second);
    for (auto it = elements.begin(); it != elements.end(); ++it)
        obj->addElement(*it);
}

} }
