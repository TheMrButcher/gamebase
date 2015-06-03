#include "TypePresentation.h"
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase { namespace editor {

void TypePresentation::serialize(Serializer& s) const
{
    s << "name" << name << "nameInUI" << nameInUI << "isAbstract" << isAbstract
        << "parents" << parents << "properties" << properties;
}

std::unique_ptr<IObject> deserializeTypePresentation(Deserializer& deserializer)
{
    std::unique_ptr<TypePresentation> result(new TypePresentation());
    deserializer >> "name" >> result->name >> "nameInUI" >> result->nameInUI
        >> "isAbstract" >> result->isAbstract  >> "parents" >> result->parents
        >> "properties" >> result->properties;
    return std::move(result);
}

REGISTER_CLASS(TypePresentation);

} }
