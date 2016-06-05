#include <stdafx.h>
#include <gamebase/impl/skin/impl/AnimatedButtonSkin.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void AnimatedButtonSkin::setFixedBox(float width, float height)
{
    m_box = std::make_shared<FixedBox>(width, height);
}

std::unique_ptr<IObject> deserializeAnimatedButtonSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeGeometry>, geometry);
    std::unique_ptr<AnimatedButtonSkin> result(new AnimatedButtonSkin(box, geometry));
    deserializeAnimatedObjectElements(deserializer, result.get());
    return std::move(result);
}

REGISTER_CLASS(AnimatedButtonSkin);

} }
