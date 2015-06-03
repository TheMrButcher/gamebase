#include "TypePresentation.h"
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase { namespace editor {

void TypePresentation::serialize(Serializer& s) const
{
    s << "isAbstract" << isAbstract << "name" << name << "nameInUI" << nameInUI
        << "parents" << parents << "properties" << properties;
}

std::unique_ptr<IObject> deserializeTypePresentation(Deserializer& deserializer)
{
    std::unique_ptr<TypePresentation> result(new TypePresentation());
    deserializer >> "isAbstract" >> result->isAbstract >> "name" >> result->name
        >> "nameInUI" >> result->nameInUI >> "parents" >> result->parents
        >> "properties" >> result->properties;
    return std::move(result);
}

REGISTER_CLASS(TypePresentation);

} }
