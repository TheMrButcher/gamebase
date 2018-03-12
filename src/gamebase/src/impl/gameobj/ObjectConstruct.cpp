/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameobj/ObjectConstruct.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

ObjectConstruct::ObjectConstruct(
    const std::shared_ptr<Drawable>& drawable,
    const std::shared_ptr<PositionElement>& position)
    : AnimatedObjectConstruct(drawable, position)
{}

void ObjectConstruct::setFixedBox(float width, float height)
{
    AnimatedObjectConstruct::setFixedBox(width, height);
    if (m_findElem)
        m_findElem->setBox(box());
}

void ObjectConstruct::setSelector(const std::shared_ptr<SelectableElement>& selectable)
{
    m_selectionElem = selectable;
    if (m_selectionElem)
        m_selectionElem->setTransitionCallback(
            [this](SelectionState::Enum value) { runTransitionAnimation(value); });
}

void ObjectConstruct::registerObject(PropertiesRegisterBuilder* builder)
{
    AnimatedObjectConstruct::registerObject(builder);
    if (m_findElem)
        builder->registerObject(m_findElem.get());
    if (m_selectionElem)
        builder->registerObject(m_selectionElem.get());
}

void ObjectConstruct::serialize(Serializer& s) const
{
    AnimatedObjectConstruct::serialize(s);
    s << "finder" << m_findElem << "selector" << m_selectionElem
        << "transitionAnimations" << m_transitionAnimations;
}

void ObjectConstruct::runTransitionAnimation(SelectionState::Enum state)
{
    auto it = m_transitionAnimations.find(state);
    if (it == m_transitionAnimations.end())
    {
        it = m_transitionAnimations.find(SelectionState::None);
        if (it == m_transitionAnimations.end())
            return;
    }
    resetChannel(TRANSITION_CHANNEL);
    runAnimation(it->second, TRANSITION_CHANNEL);
}

std::unique_ptr<IObject> deserializeObjectConstruct(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<Drawable>, skin);
    DESERIALIZE(std::shared_ptr<PositionElement>, position);
    
    typedef std::map<std::string, std::shared_ptr<IAnimation>> Animations;
    DESERIALIZE(Animations, animations);
    typedef std::map<SelectionState::Enum, std::string> TransitionAnimations;
    DESERIALIZE(TransitionAnimations, transitionAnimations);

    DESERIALIZE(int, id);
    DESERIALIZE(std::shared_ptr<FindableElement>, finder);
    DESERIALIZE(std::shared_ptr<SelectableElement>, selector);

    std::unique_ptr<ObjectConstruct> result(new ObjectConstruct(skin, position));

    for (auto it = animations.begin(); it != animations.end(); ++it)
        result->addAnimation(it->first, it->second);
    for (auto it = transitionAnimations.begin(); it != transitionAnimations.end(); ++it)
        result->setTransitionAnimation(it->first, it->second);

    result->setFinder(finder);
    result->setSelector(selector);
    result->setID(id);

    return std::move(result);
}

REGISTER_CLASS(ObjectConstruct);

} }
