#include <stdafx.h>
#include <gamebase/engine/ObjectConstruct.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {

ObjectConstruct::ObjectConstruct(
    const std::shared_ptr<IDrawable>& drawable,
    const std::shared_ptr<PositionElement>& position)
    : AnimatedObjectConstruct(drawable, position)
{}

void ObjectConstruct::setSelector(const std::shared_ptr<SelectableElement>& selectable)
{
    m_selectionElem = selectable;
    if (m_selectionElem)
        m_selectionElem->setTransitionCallback(
            std::bind(&ObjectConstruct::runTransitionAnimation, this, std::placeholders::_1));
}

void ObjectConstruct::registerObject(PropertiesRegisterBuilder* builder)
{
    AnimatedObjectConstruct::registerObject(builder);
    if (m_findElem)
        builder->registerObject("finder", m_findElem.get());
    if (m_selectionElem)
        builder->registerObject("selector", m_selectionElem.get());
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
    DESERIALIZE(std::shared_ptr<IDrawable>, skin);
    DESERIALIZE(std::shared_ptr<PositionElement>, position);
    
    typedef std::map<std::string, std::shared_ptr<IAnimation>> Animations;
    DESERIALIZE(Animations, animations);
    typedef std::map<SelectionState::Enum, std::string> TransitionAnimations;
    DESERIALIZE(TransitionAnimations, transitionAnimations);

    DESERIALIZE(int, id);
    DESERIALIZE(std::shared_ptr<MovableElement>, mover);
    DESERIALIZE(std::shared_ptr<FindableElement>, finder);
    DESERIALIZE(std::shared_ptr<SelectableElement>, selector);

    std::unique_ptr<ObjectConstruct> result(new ObjectConstruct(skin, position));

    for (auto it = animations.begin(); it != animations.end(); ++it)
        result->addAnimation(it->first, it->second);
    for (auto it = transitionAnimations.begin(); it != transitionAnimations.end(); ++it)
        result->setTransitionAnimation(it->first, it->second);

    result->setMover(mover);
    result->setFinder(finder);
    result->setSelector(selector);
    result->setID(id);

    return std::move(result);
}

REGISTER_CLASS(ObjectConstruct);

}
