#include "PropertyPresentation.h"
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase { namespace editor {

#define START_PROPERTY_PRESENTATION_CLASS_SERIALIZER(ClassName) \
    void ClassName::serialize(Serializer& s) const \
    { \
        IPropertyPresentation::serialize(s);

#define START_PROPERTY_PRESENTATION_CLASS_DESERIALIZER(ClassName) \
    std::unique_ptr<IObject> deserialize##ClassName(Deserializer& deserializer) \
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


void IPropertyPresentation::serialize(Serializer& s) const
{
    s << "nameInUI" << nameInUI;
}

REGISTER_PROPERTY_PRESENTATION_CLASS(PrimitivePropertyPresentation, type);
REGISTER_PROPERTY_PRESENTATION_CLASS(EnumPropertyPresentation, type);
REGISTER_PROPERTY_PRESENTATION_CLASS(PrimitiveArrayPresentation, type);
REGISTER_PROPERTY_PRESENTATION_CLASS2(ObjectPresentation, baseType, canBeEmpty);
REGISTER_PROPERTY_PRESENTATION_CLASS(ArrayPresentation, elementType);
REGISTER_PROPERTY_PRESENTATION_CLASS2(MapPresentation, keyType, valueType);
/*
void PrimitivePropertyPresentation::serialize(Serializer& s) const
{
    IPropertyPresentation::serialize(s);
    s << "type" << type;
}

void EnumPropertyPresentation::serialize(Serializer& s) const
{
    IPropertyPresentation::serialize(s);
    s << "type" << type;
}

void PrimitiveArrayPresentation::serialize(Serializer& s) const
{
    IPropertyPresentation::serialize(s);
    s << "type" << type;
}

void ObjectPresentation::serialize(Serializer& s) const
{
    IPropertyPresentation::serialize(s);
    s << "baseType" << baseType << "canBeEmpty" << canBeEmpty;
}

void ArrayPresentation::serialize(Serializer& s) const
{
    IPropertyPresentation::serialize(s);
    s << "elementType" << elementType;
}

void MapPresentation::serialize(Serializer& s) const
{
    IPropertyPresentation::serialize(s);
    s << "keyType" << keyType << "valueType" << valueType;
}

std::unique_ptr<IObject> deserializePrimitivePropertyPresentation(Deserializer& deserializer)
{
    std::unique_ptr<PrimitivePropertyPresentation> result(new PrimitivePropertyPresentation());
    deserializer >> "nameInUI" >> result->nameInUI >> "type" >> result->type;
    return std::move(result);
}

std::unique_ptr<IObject> deserializeEnumPropertyPresentation(Deserializer& deserializer)
{
    std::unique_ptr<EnumPropertyPresentation> result(new EnumPropertyPresentation());
    deserializer >> "nameInUI" >> result->nameInUI >> "type" >> result->type;
    return std::move(result);
}

std::unique_ptr<IObject> deserializePrimitiveArrayPresentation(Deserializer& deserializer)
{
    std::unique_ptr<PrimitiveArrayPresentation> result(new PrimitiveArrayPresentation());
    deserializer >> "nameInUI" >> result->nameInUI >> "type" >> result->type;
    return std::move(result);
}

std::unique_ptr<IObject> deserializeObjectPresentation(Deserializer& deserializer)
{
    std::unique_ptr<ObjectPresentation> result(new ObjectPresentation());
    deserializer >> "nameInUI" >> result->nameInUI >> "baseType" >> result->baseType
        >> "canBeEmpty" >> result->canBeEmpty;
    return std::move(result);
}

std::unique_ptr<IObject> deserializeObjectPresentation(Deserializer& deserializer)
{
    std::unique_ptr<ObjectPresentation> result(new ObjectPresentation());
    deserializer >> "nameInUI" >> result->nameInUI >> "baseType" >> result->baseType
        >> "canBeEmpty" >> result->canBeEmpty;
    return std::move(result);
}

REGISTER_CLASS(EnumPresentation);*/

} }
