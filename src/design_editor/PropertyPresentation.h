/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "PrimitiveType.h"
#include "IVisibilityCondition.h"
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/impl/serial/constants.h>
#include <string>
#include <memory>
#include <set>

namespace gamebase { namespace editor {

struct PropertyPresentation {
    enum Type {
        Primitive,
        Enum,
        PrimitiveArray,
        Object,
        Array,
        Map,
		SpecialString
    };
};

struct SpecialString {
	enum Type {
		Font,
		ImagePath
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
    const IVisibilityCondition* visibilityCond;
};

class IIndexablePropertyPresentation : public IPropertyPresentation {
};

class PrimitivePropertyPresentation : public IIndexablePropertyPresentation {
public:
    PrimitivePropertyPresentation() : type(PrimitiveType::Float) {}

    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Primitive; }

    virtual void serialize(impl::Serializer& serializer) const override;

	PrimitiveType::Enum type;

private:
    typedef IPropertyPresentation BaseType;
};

class EnumPropertyPresentation : public IIndexablePropertyPresentation {
public:
    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Enum; }

    virtual void serialize(impl::Serializer& serializer) const override;

    std::string type;

private:
    typedef IPropertyPresentation BaseType;
};

class SpecialStringPresentation : public IPropertyPresentation {
public:
	SpecialStringPresentation() : type(SpecialString::Font) {}

    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::SpecialString; }

    virtual void serialize(impl::Serializer& serializer) const override;

	SpecialString::Type type;

private:
    typedef IPropertyPresentation BaseType;
};

class PrimitiveArrayPresentation : public IPropertyPresentation {
public:
    PrimitiveArrayPresentation() : type(impl::SerializationTag::Vec2) {}

    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::PrimitiveArray; }

    virtual void serialize(impl::Serializer& serializer) const override;

    impl::SerializationTag::Type type;

private:
    typedef IPropertyPresentation BaseType;
};

class IComplexPropertyPresentation : public IPropertyPresentation {
public:
    IComplexPropertyPresentation() : isInline(false) {}

    virtual void serialize(impl::Serializer& serializer) const override;
    
    bool isInline;
};

class ObjectPresentation : public IComplexPropertyPresentation {
public:
    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Object; }

    ObjectPresentation() : canBeEmpty(false) {}

    virtual void serialize(impl::Serializer& serializer) const override;

    std::string baseType;
    bool canBeEmpty;
    std::set<std::string> tags;

private:
    typedef IComplexPropertyPresentation BaseType;
};

class ArrayPresentation : public IComplexPropertyPresentation {
public:
    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Array; }

    virtual void serialize(impl::Serializer& serializer) const override;

    std::shared_ptr<IPropertyPresentation> elementType;

private:
    typedef IComplexPropertyPresentation BaseType;
};

class MapPresentation : public IComplexPropertyPresentation {
public:
    virtual PropertyPresentation::Type presentationType() const override { return PropertyPresentation::Map; }

    virtual void serialize(impl::Serializer& serializer) const override;

    std::shared_ptr<IIndexablePropertyPresentation> keyType;
    std::shared_ptr<IPropertyPresentation> valueType;

private:
    typedef IComplexPropertyPresentation BaseType;
};

} }
