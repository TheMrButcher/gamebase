#include "EnumPresentation.h"
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase { namespace editor {

void EnumPresentation::serialize(Serializer& s) const
{
    s << "name" << name << "nameInUI" << nameInUI << "values" << values;
}

std::unique_ptr<IObject> deserializeEnumPresentation(Deserializer& deserializer)
{
    std::unique_ptr<EnumPresentation> result(new EnumPresentation());
    deserializer >> "name" >> result->name >> "nameInUI" >> result->nameInUI
        >> "values" >> result->values;
    return std::move(result);
}

REGISTER_CLASS(EnumPresentation);

} }
