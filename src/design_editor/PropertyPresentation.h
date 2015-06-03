#pragma once

#include "PrimitiveType.h"
#include <gamebase/serial/ISerializable.h>
#include <gamebase/serial/constants.h>
#include <string>
#include <memory>

namespace gamebase { namespace editor {

class IPropertyPresentation : public ISerializable {
public:
    virtual void serialize(Serializer& serializer) const override;

    std::string nameInUI;
};

class IIndexablePropertyPresentation : public IPropertyPresentation {
};

class PrimitivePropertyPresentation : public IIndexablePropertyPresentation {
public:
    virtual void serialize(Serializer& serializer) const override;

    PrimitiveType::Enum type;
};

class EnumPropertyPresentation : public IIndexablePropertyPresentation {
public:
    virtual void serialize(Serializer& serializer) const override;

    std::string type;
};

class PrimitiveArrayPresentation : public IPropertyPresentation {
public:
    virtual void serialize(Serializer& serializer) const override;

    SerializationTag::Type type;
};

class ObjectPresentation : public IPropertyPresentation {
public:
    ObjectPresentation() : canBeEmpty(false) {}

    virtual void serialize(Serializer& serializer) const override;

    std::string baseType;
    bool canBeEmpty;
};

class ArrayPresentation : public IPropertyPresentation {
public:
    virtual void serialize(Serializer& serializer) const override;

    std::shared_ptr<IPropertyPresentation> elementType;
};

class MapPresentation : public IPropertyPresentation {
public:
    virtual void serialize(Serializer& serializer) const override;

    std::shared_ptr<IIndexablePropertyPresentation> keyType;
    std::shared_ptr<IPropertyPresentation> valueType;
};

} }
