#include <stdafx.h>
#include <gamebase/engine/AnimatedButtonSkin.h>
#include <gamebase/engine/FixedBox.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

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

}
