#include "TypePresentation.h"
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/JsonDeserializer.h>

namespace gamebase { namespace editor {

void TypePresentation::serialize(impl::Serializer& s) const
{
    s << "name" << name << "nameInUI" << nameInUI << "isAbstract" << isAbstract
        << "parents" << parents << "properties" << properties
        << "pathToPatternValue" << pathToPatternValue;
}

std::unique_ptr<impl::IObject> deserializeTypePresentation(impl::Deserializer& deserializer)
{
    std::unique_ptr<TypePresentation> result(new TypePresentation());
    deserializer >> "name" >> result->name >> "nameInUI" >> result->nameInUI
        >> "isAbstract" >> result->isAbstract  >> "parents" >> result->parents
        >> "properties" >> result->properties >> "pathToPatternValue" >> result->pathToPatternValue;
    return std::move(result);
}

REGISTER_CLASS(TypePresentation);

} }
