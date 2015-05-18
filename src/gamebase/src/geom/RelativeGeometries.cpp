#include <stdafx.h>
#include <gamebase/geom/IdenticGeometry.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

IObject* deserializeIdenticGeometry(Deserializer&)
{
    return new IdenticGeometry();
}

REGISTER_CLASS(IdenticGeometry);

}
