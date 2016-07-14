/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "PrimitiveType.h"
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/impl/serial/constants.h>
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

class IPropertyPresentation : public impl::ISerializable {
public:
    virtual PropertyPresentation::Type presentationType() const = 0;

    virtual void serialize(impl::Serializer& serializer) const override;

    std::string nameInUI;
};

class IIndexablePropertyPresentation : public IPropertyPresentation {
};

class PrimitivePropertyPresentation : public IIndexablePropertyPresentation {
public:
    PrimitivePropertyPresentation() : type(PrimitiveType::Float) {}

    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Primitive; }

    virtual void serialize(impl::Serializer& serializer) const override;

    PrimitiveType::Enum type;
};

class EnumPropertyPresentation : public IIndexablePropertyPresentation {
public:
    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Enum; }

    virtual void serialize(impl::Serializer& serializer) const override;

    std::string type;
};

class PrimitiveArrayPresentation : public IPropertyPresentation {
public:
    PrimitiveArrayPresentation() : type(impl::SerializationTag::Vec2) {}

    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::PrimitiveArray; }

    virtual void serialize(impl::Serializer& serializer) const override;

    impl::SerializationTag::Type type;
};

class ObjectPresentation : public IPropertyPresentation {
public:
    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Object; }

    ObjectPresentation() : canBeEmpty(false) {}

    virtual void serialize(impl::Serializer& serializer) const override;

    std::string baseType;
    bool canBeEmpty;
};

class ArrayPresentation : public IPropertyPresentation {
public:
    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Array; }

    virtual void serialize(impl::Serializer& serializer) const override;

    std::shared_ptr<IPropertyPresentation> elementType;
};

class MapPresentation : public IPropertyPresentation {
public:
    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Map; }

    virtual void serialize(impl::Serializer& serializer) const override;

    std::shared_ptr<IIndexablePropertyPresentation> keyType;
    std::shared_ptr<IPropertyPresentation> valueType;
};

} }
