#pragma once

#include "PrimitiveType.h"
#include <gamebase/serial/ISerializable.h>
#include <gamebase/serial/constants.h>
#include <string>
#include <memory>

namespace gamebase { namespace editor {

struct PropertyPresentation {
    enum Type {
        Primitive,
        Enum,
        PrimitiveArray,
        Object,
        Array,
        Map
    };
};

inline bool isPrimitive(PropertyPresentation::Type type)
{
    return type == PropertyPresentation::Primitive
        || type == PropertyPresentation::Enum;
}

class IPropertyPresentation : public ISerializable {
public:
    virtual PropertyPresentation::Type presentationType() const = 0;

    virtual void serialize(Serializer& serializer) const override;

    std::string nameInUI;
};

class IIndexablePropertyPresentation : public IPropertyPresentation {
};

class PrimitivePropertyPresentation : public IIndexablePropertyPresentation {
public:
    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Primitive; }

    virtual void serialize(Serializer& serializer) const override;

    PrimitiveType::Enum type;
};

class EnumPropertyPresentation : public IIndexablePropertyPresentation {
public:
    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Enum; }

    virtual void serialize(Serializer& serializer) const override;

    std::string type;
};

class PrimitiveArrayPresentation : public IPropertyPresentation {
public:
    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::PrimitiveArray; }

    virtual void serialize(Serializer& serializer) const override;

    SerializationTag::Type type;
};

class ObjectPresentation : public IPropertyPresentation {
public:
    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Object; }

    ObjectPresentation() : canBeEmpty(false) {}

    virtual void serialize(Serializer& serializer) const override;

    std::string baseType;
    bool canBeEmpty;
};

class ArrayPresentation : public IPropertyPresentation {
public:
    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Array; }

    virtual void serialize(Serializer& serializer) const override;

    std::shared_ptr<IPropertyPresentation> elementType;
};

class MapPresentation : public IPropertyPresentation {
public:
    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Map; }

    virtual void serialize(Serializer& serializer) const override;

    std::shared_ptr<IIndexablePropertyPresentation> keyType;
    std::shared_ptr<IPropertyPresentation> valueType;
};

} }
