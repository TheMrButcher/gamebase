#include <stdafx.h>
#include <gamebase/impl/geom/IdenticGeometry.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

std::unique_ptr<IObject> deserializeIdenticGeometry(Deserializer&)
{
    return std::unique_ptr<IObject>(new IdenticGeometry());
}

REGISTER_CLASS(IdenticGeometry);

} }
