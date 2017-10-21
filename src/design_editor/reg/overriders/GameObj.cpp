/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "GameObj.h"
#include <reg/overriders/SelectableElements.h>

namespace gamebase { namespace editor {

namespace {
const std::unordered_map<std::string, std::shared_ptr<impl::IAnimation>> EMPTY_ANIMATIONS;
const std::map<impl::SelectionState::Enum, std::string> EMPTY_TRANSITION_ANIMATIONS;
const std::shared_ptr<CommonSelectableElement> DEFAULT_SELECTABLE_ELEM =
    std::make_shared<CommonSelectableElement>();

template <typename T>
std::unique_ptr<T> createGameObj(
    const std::shared_ptr<impl::Drawable>& skin,
    const std::shared_ptr<impl::PositionElement>& position,
    int id)
{
    std::unique_ptr<T> result(new T(skin, position));
    result->setID(id);
    return result;
}

void setAnimations(
    impl::AnimatedObjectConstruct* obj,
    const std::map<std::string, std::shared_ptr<impl::IAnimation>>& animations)
{
    for (auto it = animations.begin(); it != animations.end(); ++it)
        obj->addAnimation(it->first, it->second);
}
}

void serializeInactiveObjectConstruct(const impl::IObject* obj, impl::Serializer& s)
{
    if (auto gobj = dynamic_cast<const impl::InactiveObjectConstruct*>(obj)) {
        gobj->serialize(s);
        s   << "animations" << EMPTY_ANIMATIONS
            << "transitionAnimations" << EMPTY_TRANSITION_ANIMATIONS
            << "selector" << DEFAULT_SELECTABLE_ELEM
            << "activeGeom" << std::shared_ptr<impl::IRelativeGeometry>();
    } else {
        THROW_EX() << "Serializer expected InactiveObjectConstruct as input";
    }
}

void serializeAnimatedObjectConstruct(const impl::IObject* obj, impl::Serializer& s)
{
    if (auto gobj = dynamic_cast<const impl::AnimatedObjectConstruct*>(obj)) {
        gobj->serialize(s);
        s   << "transitionAnimations" << EMPTY_TRANSITION_ANIMATIONS
            << "selector" << DEFAULT_SELECTABLE_ELEM
            << "activeGeom" << std::shared_ptr<impl::IRelativeGeometry>();
    } else {
        THROW_EX() << "Serializer expected AnimatedObjectConstruct as input";
    }
}

void serializeObjectConstruct(const impl::IObject* obj, impl::Serializer& s)
{
    if (auto gobj = dynamic_cast<const impl::ObjectConstruct*>(obj)) {
        gobj->impl::AnimatedObjectConstruct::serialize(s);
        s   << "transitionAnimations" << gobj->transitionAnimations()
            << "selector" << gobj->selector();
        if (auto findabledGeomElem = dynamic_cast<const impl::FindableGeometryElement*>(gobj->finder().get())) {
            s << "activeGeom" << findabledGeomElem->geometry();
        } else {
            s << "activeGeom" << std::shared_ptr<impl::IRelativeGeometry>();
        }
    } else {
        THROW_EX() << "Serializer expected ObjectConstruct as input";
    }
}

std::unique_ptr<impl::IObject> deserializeObjectConstruct(impl::Deserializer& deserializer)
{
    using namespace gamebase::impl;

    DESERIALIZE(std::shared_ptr<Drawable>, skin);
    DESERIALIZE(std::shared_ptr<PositionElement>, position);
    
    typedef std::map<std::string, std::shared_ptr<IAnimation>> Animations;
    DESERIALIZE(Animations, animations);
    typedef std::map<SelectionState::Enum, std::string> TransitionAnimations;
    DESERIALIZE(TransitionAnimations, transitionAnimations);

    DESERIALIZE(int, id);
    DESERIALIZE(std::shared_ptr<SelectableElement>, selector);

    std::shared_ptr<FindableElement> finder;
    if (deserializer.hasMember("finder")) {
        deserializer >> "finder" >> finder;
    } else {
        DESERIALIZE(std::shared_ptr<IRelativeGeometry>, activeGeom);
        if (activeGeom)
            finder = std::make_shared<FindableGeometryElement>(activeGeom);
    }

    if (transitionAnimations.empty() && !finder && !selector) {
        if (animations.empty()) {
            return createGameObj<InactiveObjectConstruct>(skin, position, id);
        }
        auto result = createGameObj<AnimatedObjectConstruct>(skin, position, id);
        setAnimations(result.get(), animations);
        return std::move(result);
    }
    
    auto result = createGameObj<ObjectConstruct>(skin, position, id);
    setAnimations(result.get(), animations);
    for (auto it = transitionAnimations.begin(); it != transitionAnimations.end(); ++it)
        result->setTransitionAnimation(it->first, it->second);
    result->setFinder(finder);
    result->setSelector(selector);
    return std::move(result);
}

} }
