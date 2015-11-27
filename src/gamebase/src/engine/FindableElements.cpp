#include <stdafx.h>
#include <gamebase/engine/FindableGeometryElement.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

FindableGeometryElement::FindableGeometryElement(
    const std::shared_ptr<IRelativeGeometry>& geom)
    : FindableGeometry(nullptr, geom)
{}

void FindableGeometryElement::serialize(Serializer& s) const
{
    s << "geom" << m_findableGeom;
}

std::unique_ptr<IObject> deserializeFindableGeometryElement(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeGeometry>, geom);
    return std::unique_ptr<FindableGeometryElement>(new FindableGeometryElement(geom));
}

REGISTER_CLASS(FindableGeometryElement);

}
