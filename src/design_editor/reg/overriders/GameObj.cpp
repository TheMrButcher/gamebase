/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "GameObj.h"

namespace gamebase { namespace editor {

void serializeObjectConstruct(const impl::IObject* obj, impl::Serializer& s)
{
    std::cout << "serializeObjectConstruct" << std::endl;
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
    std::cout << "deserializeObjectConstruct" << std::endl;
    using namespace gamebase::impl;

    DESERIALIZE(std::shared_ptr<IDrawable>, skin);
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

} }
