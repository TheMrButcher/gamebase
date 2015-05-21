#include <stdafx.h>
#include <gamebase/engine/AnimatedButtonSkin.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/geom/IdenticGeometry.h>

namespace gamebase {

AnimatedButtonSkin::AnimatedButtonSkin(
    const std::shared_ptr<IRelativeBox>& box)
    : m_box(box)
    , m_geom(std::make_shared<IdenticGeometry>())
{}

void AnimatedButtonSkin::serialize(Serializer& s) const
{
    s << "box" << m_box << "geometry" << m_geom
        << "animations" << m_animations << "elements" << m_skinElements.objects();
}

IObject* deserializeAnimatedButtonSkin(Deserializer& deserializer)
{
    typedef std::map<SelectionState::Enum, std::shared_ptr<IAnimation>> Animations;
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeGeometry>, geometry);
    DESERIALIZE(Animations, animations);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, elements);

    auto* result = new AnimatedButtonSkin(box, geometry);
    for (auto it = animations.begin(); it != animations.end(); ++it)
        result->setTransitAnimation(it->first, it->second);
    for (auto it = elements.begin(); it != elements.end(); ++it)
        result->addElement(*it);
    return result;
}

REGISTER_CLASS(AnimatedButtonSkin);

}
