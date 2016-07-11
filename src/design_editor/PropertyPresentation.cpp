#include "PropertyPresentation.h"
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace editor {

#define START_PROPERTY_PRESENTATION_CLASS_SERIALIZER(ClassName) \
    void ClassName::serialize(impl::Serializer& s) const \
    { \
        IPropertyPresentation::serialize(s);

#define START_PROPERTY_PRESENTATION_CLASS_DESERIALIZER(ClassName) \
    std::unique_ptr<impl::IObject> deserialize##ClassName(impl::Deserializer& deserializer) \
    { \
        std::unique_ptr<ClassName> result(new ClassName()); \
        deserializer >> "nameInUI" >> result->nameInUI

#define FINISH_PROPERTY_PRESENTATION_CLASS_DESERIALIZER() \
        return std::move(result); \
    }

#define REGISTER_PROPERTY_PRESENTATION_CLASS(ClassName, member1) \
    START_PROPERTY_PRESENTATION_CLASS_SERIALIZER(ClassName) \
        s << #member1 << member1; \
    } \
    START_PROPERTY_PRESENTATION_CLASS_DESERIALIZER(ClassName) \
        >> #member1 >> result->member1; \
    FINISH_PROPERTY_PRESENTATION_CLASS_DESERIALIZER() \
    REGISTER_CLASS(ClassName)

#define REGISTER_PROPERTY_PRESENTATION_CLASS2(ClassName, member1, member2) \
    START_PROPERTY_PRESENTATION_CLASS_SERIALIZER(ClassName) \
    s << #member1 << member1 << #member2 << member2; \
    } \
    START_PROPERTY_PRESENTATION_CLASS_DESERIALIZER(ClassName) \
    >> #member1 >> result->member1 >> #member2 >> result->member2; \
    FINISH_PROPERTY_PRESENTATION_CLASS_DESERIALIZER() \
    REGISTER_CLASS(ClassName)


void IPropertyPresentation::serialize(impl::Serializer& s) const
{
    s << "nameInUI" << nameInUI;
}

REGISTER_PROPERTY_PRESENTATION_CLASS(PrimitivePropertyPresentation, type);
REGISTER_PROPERTY_PRESENTATION_CLASS(EnumPropertyPresentation, type);
REGISTER_PROPERTY_PRESENTATION_CLASS(PrimitiveArrayPresentation, type);
REGISTER_PROPERTY_PRESENTATION_CLASS2(ObjectPresentation, baseType, canBeEmpty);
REGISTER_PROPERTY_PRESENTATION_CLASS(ArrayPresentation, elementType);
REGISTER_PROPERTY_PRESENTATION_CLASS2(MapPresentation, keyType, valueType);

} }
