#include <stdafx.h>
#include <gamebase/geom/IdenticGeometry.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

std::unique_ptr<IObject> deserializeIdenticGeometry(Deserializer&)
{
    return std::unique_ptr<IObject>(new IdenticGeometry());
}

REGISTER_CLASS(IdenticGeometry);

}
