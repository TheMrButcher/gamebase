/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "PropertyPresentation.h"
#include "VisibilityConditions.h"
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace editor {

#define START_PROPERTY_PRESENTATION_CLASS_SERIALIZER(ClassName) \
    void ClassName::serialize(impl::Serializer& s) const \
    { \
        BaseType::serialize(s)

#define FINISH_PROPERTY_PRESENTATION_CLASS_SERIALIZER() }

#define START_PROPERTY_PRESENTATION_CLASS_DESERIALIZER(ClassName) \
    std::unique_ptr<impl::IObject> deserialize##ClassName(impl::Deserializer& deserializer) \
    { \
        std::unique_ptr<ClassName> result(new ClassName()); \
        std::string visibilityCondName; \
        if (deserializer.hasMember("visibilityCond")) \
            deserializer >> "visibilityCond" >> visibilityCondName; \
        result->visibilityCond = VisibilityConditions::instance().get(visibilityCondName); \
        deserializer >> "nameInUI" >> result->nameInUI

#define FINISH_PROPERTY_PRESENTATION_CLASS_DESERIALIZER() \
        return std::move(result); \
    }

#define SERIALIZE_MEMBER(member) s << #member << member
#define DESERIALIZE_MEMBER(member) deserializer >> #member >> result->member

#define REGISTER_PROPERTY_PRESENTATION_CLASS(ClassName, member1) \
    START_PROPERTY_PRESENTATION_CLASS_SERIALIZER(ClassName); \
        SERIALIZE_MEMBER(member1); \
    FINISH_PROPERTY_PRESENTATION_CLASS_SERIALIZER(); \
    START_PROPERTY_PRESENTATION_CLASS_DESERIALIZER(ClassName); \
        DESERIALIZE_MEMBER(member1); \
    FINISH_PROPERTY_PRESENTATION_CLASS_DESERIALIZER() \
    REGISTER_CLASS(ClassName)

void IPropertyPresentation::serialize(impl::Serializer& s) const
{
    s << "nameInUI" << nameInUI
        << "visibilityCond" << (visibilityCond ? visibilityCond->name() : "");
}

void IComplexPropertyPresentation::serialize(impl::Serializer& s) const
{
    IPropertyPresentation::serialize(s);
    s << "isInline" << isInline;
}

REGISTER_PROPERTY_PRESENTATION_CLASS(PrimitivePropertyPresentation, type);
REGISTER_PROPERTY_PRESENTATION_CLASS(EnumPropertyPresentation, type);
REGISTER_PROPERTY_PRESENTATION_CLASS(SpecialStringPresentation, type);
REGISTER_PROPERTY_PRESENTATION_CLASS(PrimitiveArrayPresentation, type);

START_PROPERTY_PRESENTATION_CLASS_SERIALIZER(ArrayPresentation);
    SERIALIZE_MEMBER(elementType);
FINISH_PROPERTY_PRESENTATION_CLASS_SERIALIZER();
START_PROPERTY_PRESENTATION_CLASS_DESERIALIZER(ArrayPresentation);
    DESERIALIZE_MEMBER(elementType);
    if (deserializer.hasMember("isInline"))
        DESERIALIZE_MEMBER(isInline);
FINISH_PROPERTY_PRESENTATION_CLASS_DESERIALIZER();
REGISTER_CLASS(ArrayPresentation);

START_PROPERTY_PRESENTATION_CLASS_SERIALIZER(MapPresentation);
    SERIALIZE_MEMBER(keyType);
    SERIALIZE_MEMBER(valueType);
FINISH_PROPERTY_PRESENTATION_CLASS_SERIALIZER();
START_PROPERTY_PRESENTATION_CLASS_DESERIALIZER(MapPresentation);
    DESERIALIZE_MEMBER(keyType); DESERIALIZE_MEMBER(valueType);
    if (deserializer.hasMember("isInline"))
        DESERIALIZE_MEMBER(isInline);
FINISH_PROPERTY_PRESENTATION_CLASS_DESERIALIZER();
REGISTER_CLASS(MapPresentation);

START_PROPERTY_PRESENTATION_CLASS_SERIALIZER(ObjectPresentation);
    SERIALIZE_MEMBER(baseType);
    SERIALIZE_MEMBER(canBeEmpty);
    s << "tags" << std::vector<std::string>(tags.begin(), tags.end());
FINISH_PROPERTY_PRESENTATION_CLASS_SERIALIZER();
START_PROPERTY_PRESENTATION_CLASS_DESERIALIZER(ObjectPresentation);
    DESERIALIZE_MEMBER(baseType); DESERIALIZE_MEMBER(canBeEmpty);
    if (deserializer.hasMember("isInline"))
        DESERIALIZE_MEMBER(isInline);
    if (deserializer.hasMember("tags")) {
        DESERIALIZE(std::vector<std::string>, tags);
        result->tags.insert(tags.begin(), tags.end());
    }
FINISH_PROPERTY_PRESENTATION_CLASS_DESERIALIZER();
REGISTER_CLASS(ObjectPresentation);

} }
