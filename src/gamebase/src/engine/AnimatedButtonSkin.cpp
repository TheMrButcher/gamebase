#include <stdafx.h>
#include <gamebase/engine/AnimatedButtonSkin.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

IObject* deserializeAnimatedButtonSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeGeometry>, geometry);
    return deserializeAnimatedObjectElements(deserializer, new AnimatedButtonSkin(box, geometry));
}

REGISTER_CLASS(AnimatedButtonSkin);

}
