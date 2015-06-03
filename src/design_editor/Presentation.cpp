#include "Presentation.h"
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/text/Conversion.h>

namespace gamebase { namespace editor {

void Presentation::addEnum(const std::shared_ptr<EnumPresentation>& enumPresentation)
{
    m_enums.push_back(enumPresentation);
    m_enumMap[enumPresentation->name] = enumPresentation;
}

void Presentation::addType(const std::shared_ptr<TypePresentation>& typePresentation)
{
    m_types.push_back(typePresentation);
    m_typeMap[typePresentation->name] = typePresentation;
    for (auto it = typePresentation->parents.begin(); it != typePresentation->parents.end(); ++it)
        m_derivedTypesMap[*it].push_back(typePresentation);
}

std::shared_ptr<EnumPresentation> Presentation::enumByName(
    const std::string& name) const
{
    auto it = m_enumMap.find(name);
    if (it == m_enumMap.end()) {
        std::cout << "Can't find enum by name: " << name << std::endl;
        return nullptr;
    }
    return it->second;
}

std::shared_ptr<TypePresentation> Presentation::typeByName(
    const std::string& name) const
{
    auto it = m_typeMap.find(name);
    if (it == m_typeMap.end()) {
        std::cout << "Can't find type by name: " << name << std::endl;
        return nullptr;
    }
    return it->second;
}

std::vector<std::shared_ptr<TypePresentation>> Presentation::derivedTypesByBaseTypeName(
    const std::string& name, bool excludeAbstract) const
{
    std::vector<std::shared_ptr<TypePresentation>> result;
    if (auto typePresentation = typeByName(name)) {
        if (!excludeAbstract || !typePresentation->isAbstract)
            result.push_back(typePresentation);
    }

    addDerivedTypes(result, name, excludeAbstract);
    return result;
}

void Presentation::serialize(Serializer& s) const
{
    s << "types" << m_types << "enums" << m_enums;
}

std::unique_ptr<IObject> deserializePresentation(Deserializer& deserializer)
{
    DESERIALIZE(std::vector<std::shared_ptr<TypePresentation>>, types);
    DESERIALIZE(std::vector<std::shared_ptr<EnumPresentation>>, enums);

    std::unique_ptr<Presentation> result(new Presentation());
    for (auto it = enums.begin(); it != enums.end(); ++it)
        result->addEnum(*it);
    for (auto it = types.begin(); it != types.end(); ++it)
        result->addType(*it);
    return std::move(result);
}

REGISTER_CLASS(Presentation);

void Presentation::addDerivedTypes(
    std::vector<std::shared_ptr<TypePresentation>>& result,
    const std::string& name, bool excludeAbstract) const
{
    auto it = m_derivedTypesMap.find(name);
    if (it == m_derivedTypesMap.end())
        return;
    const auto& derivedTypes = it->second;
    for (auto itDerived = derivedTypes.begin(); itDerived != derivedTypes.end(); ++itDerived) {
        const auto& typePresentation = *itDerived;
        if (!excludeAbstract || !typePresentation->isAbstract)
            result.push_back(typePresentation);
        addDerivedTypes(result, typePresentation->name, excludeAbstract);
    }
}

namespace {
std::shared_ptr<TypePresentation> addType(
    Presentation& dst, const std::string& name, const std::string& nameInUI,
    const std::vector<std::string>& parents = std::vector<std::string>())
{
    auto typePresentation = std::make_shared<TypePresentation>();
    typePresentation->name = name;
    typePresentation->nameInUI = convertToUtf8(nameInUI);
    typePresentation->parents = parents;
    dst.addType(typePresentation);
    return typePresentation;
}

std::shared_ptr<TypePresentation> addAbstractType(
    Presentation& dst, const std::string& name, const std::string& nameInUI,
    const std::vector<std::string>& parents = std::vector<std::string>())
{
    auto typePresentation = addType(dst, name, nameInUI, parents);
    typePresentation->isAbstract = true;
    return typePresentation;
}

std::shared_ptr<Presentation> createHardcodedPresentationForPresentationView()
{
    auto presentation = std::make_shared<Presentation>();
    auto& result = *presentation;

    {
        auto enumPresentation = std::make_shared<EnumPresentation>();
        enumPresentation->name = "PrimitiveType";
        enumPresentation->nameInUI = convertToUtf8("Примитивный тип");
        enumPresentation->values[PrimitiveType::Float] = "float";
        enumPresentation->values[PrimitiveType::Double] = "double";
        enumPresentation->values[PrimitiveType::Int] = "int";
        enumPresentation->values[PrimitiveType::UInt] = "unsigned int";
        enumPresentation->values[PrimitiveType::Int64] = "int64";
        enumPresentation->values[PrimitiveType::UInt64] = "unsigned int64";
        enumPresentation->values[PrimitiveType::Bool] = "bool";
        enumPresentation->values[PrimitiveType::String] = "string";
        result.addEnum(enumPresentation);
    }

    {
        auto enumPresentation = std::make_shared<EnumPresentation>();
        enumPresentation->name = "SerializationTag";
        enumPresentation->nameInUI = convertToUtf8("Примитивный массив");
        enumPresentation->values[SerializationTag::Vec2] = convertToUtf8("Вектор");
        enumPresentation->values[SerializationTag::Matrix2] = convertToUtf8("Матрица");
        enumPresentation->values[SerializationTag::Transform2] = convertToUtf8("Трансформация");
        enumPresentation->values[SerializationTag::BoundingBox] = convertToUtf8("Прямоугольник");
        enumPresentation->values[SerializationTag::Color] = convertToUtf8("Цвет");
        result.addEnum(enumPresentation);
    }

    {
        auto typePresentation = addAbstractType(result, "IPropertyPresentation", "");
        auto propertyPresentation = std::make_shared<PrimitivePropertyPresentation>();
        propertyPresentation->type = PrimitiveType::String;
        propertyPresentation->nameInUI = convertToUtf8("Название поля");
        typePresentation->properties["nameInUI"] = propertyPresentation;
    }

    addAbstractType(result, "IIndexablePropertyPresentation", "",
        std::vector<std::string>(1, "IPropertyPresentation"));

    {
        auto typePresentation = addType(result, "PrimitivePropertyPresentation", "Примитивное поле",
            std::vector<std::string>(1, "IIndexablePropertyPresentation"));
        auto propertyPresentation = std::make_shared<EnumPropertyPresentation>();
        propertyPresentation->type = "PrimitiveType";
        propertyPresentation->nameInUI = convertToUtf8("Тип");
        typePresentation->properties["type"] = propertyPresentation;
    }

    {
        auto typePresentation = addType(result, "EnumPropertyPresentation", "Поле-перечисление",
            std::vector<std::string>(1, "IIndexablePropertyPresentation"));
        auto propertyPresentation = std::make_shared<PrimitivePropertyPresentation>();
        propertyPresentation->type = PrimitiveType::String;
        propertyPresentation->nameInUI = convertToUtf8("Название перечисления");
        typePresentation->properties["type"] = propertyPresentation;
    }

    {
        auto typePresentation = addType(result, "PrimitiveArrayPresentation", "Примитивный массив",
            std::vector<std::string>(1, "IPropertyPresentation"));
        auto propertyPresentation = std::make_shared<EnumPropertyPresentation>();
        propertyPresentation->type = "SerializationTag";
        propertyPresentation->nameInUI = convertToUtf8("Тип");
        typePresentation->properties["type"] = propertyPresentation;
    }

    {
        auto typePresentation = addType(result, "ObjectPresentation", "Объект",
            std::vector<std::string>(1, "IPropertyPresentation"));
        {
            auto propertyPresentation = std::make_shared<PrimitivePropertyPresentation>();
            propertyPresentation->type = PrimitiveType::String;
            propertyPresentation->nameInUI = convertToUtf8("Базовый тип");
            typePresentation->properties["baseType"] = propertyPresentation;
        }
        {
            auto propertyPresentation = std::make_shared<PrimitivePropertyPresentation>();
            propertyPresentation->type = PrimitiveType::Bool;
            propertyPresentation->nameInUI = convertToUtf8("Может ли быть пустым");
            typePresentation->properties["canBeEmpty"] = propertyPresentation;
        }
    }

    {
        auto typePresentation = addType(result, "ArrayPresentation", "Массив",
            std::vector<std::string>(1, "IPropertyPresentation"));
        auto propertyPresentation = std::make_shared<ObjectPresentation>();
        propertyPresentation->baseType = "IPropertyPresentation";
        propertyPresentation->nameInUI = convertToUtf8("Тип элемента");
        typePresentation->properties["elementType"] = propertyPresentation;
    }

    {
        auto typePresentation = addType(result, "MapPresentation", "Отображение",
            std::vector<std::string>(1, "IPropertyPresentation"));
        {
            auto propertyPresentation = std::make_shared<ObjectPresentation>();
            propertyPresentation->baseType = "IIndexablePropertyPresentation";
            propertyPresentation->nameInUI = convertToUtf8("Тип ключа");
            typePresentation->properties["keyType"] = propertyPresentation;
        }
        {
            auto propertyPresentation = std::make_shared<ObjectPresentation>();
            propertyPresentation->baseType = "IPropertyPresentation";
            propertyPresentation->nameInUI = convertToUtf8("Тип значения");
            typePresentation->properties["valueType"] = propertyPresentation;
        }
    }

    {
        auto typePresentation = addType(result, "TypePresentation", "Тип");
        {
            auto propertyPresentation = std::make_shared<PrimitivePropertyPresentation>();
            propertyPresentation->type = PrimitiveType::Bool;
            propertyPresentation->nameInUI = convertToUtf8("Абстрактный");
            typePresentation->properties["isAbstract"] = propertyPresentation;
        }
        {
            auto propertyPresentation = std::make_shared<PrimitivePropertyPresentation>();
            propertyPresentation->type = PrimitiveType::String;
            propertyPresentation->nameInUI = convertToUtf8("ID типа");
            typePresentation->properties["name"] = propertyPresentation;
        }
        {
            auto propertyPresentation = std::make_shared<PrimitivePropertyPresentation>();
            propertyPresentation->type = PrimitiveType::String;
            propertyPresentation->nameInUI = convertToUtf8("Название типа");
            typePresentation->properties["nameInUI"] = propertyPresentation;
        }
        {
            auto propertyPresentation = std::make_shared<ArrayPresentation>();
            auto elementType = std::make_shared<PrimitivePropertyPresentation>();
            elementType->type = PrimitiveType::String;
            propertyPresentation->elementType = elementType;
            propertyPresentation->nameInUI = convertToUtf8("Базовые типы");
            typePresentation->properties["parents"] = propertyPresentation;
        }
        {
            auto propertyPresentation = std::make_shared<MapPresentation>();
            auto keyType = std::make_shared<PrimitivePropertyPresentation>();
            keyType->type = PrimitiveType::String;
            propertyPresentation->keyType = keyType;
            auto valueType = std::make_shared<ObjectPresentation>();
            valueType->baseType = "IPropertyPresentation";
            propertyPresentation->valueType = valueType;
            propertyPresentation->nameInUI = convertToUtf8("Поля");
            typePresentation->properties["properties"] = propertyPresentation;
        }
    }

    {
        auto typePresentation = addType(result, "EnumPresentation", "Перечисление");
        {
            auto propertyPresentation = std::make_shared<PrimitivePropertyPresentation>();
            propertyPresentation->type = PrimitiveType::String;
            propertyPresentation->nameInUI = convertToUtf8("ID перечисления");
            typePresentation->properties["name"] = propertyPresentation;
        }
        {
            auto propertyPresentation = std::make_shared<PrimitivePropertyPresentation>();
            propertyPresentation->type = PrimitiveType::String;
            propertyPresentation->nameInUI = convertToUtf8("Название перечисления");
            typePresentation->properties["nameInUI"] = propertyPresentation;
        }
        {
            auto propertyPresentation = std::make_shared<MapPresentation>();
            auto keyType = std::make_shared<PrimitivePropertyPresentation>();
            keyType->type = PrimitiveType::String;
            propertyPresentation->keyType = keyType;
            auto valueType = std::make_shared<PrimitivePropertyPresentation>();
            valueType->type = PrimitiveType::Int;
            propertyPresentation->valueType = valueType;
            propertyPresentation->nameInUI = convertToUtf8("Значения");
            typePresentation->properties["values"] = propertyPresentation;
        }
    }

    {
        auto typePresentation = addType(result, "Presentation", "Схема типов");
        {
            auto propertyPresentation = std::make_shared<ArrayPresentation>();
            auto elementType = std::make_shared<ObjectPresentation>();
            elementType->baseType = "TypePresentation";
            propertyPresentation->elementType = elementType;
            propertyPresentation->nameInUI = convertToUtf8("Типы");
            typePresentation->properties["types"] = propertyPresentation;
        }
        {
            auto propertyPresentation = std::make_shared<ArrayPresentation>();
            auto elementType = std::make_shared<ObjectPresentation>();
            elementType->baseType = "EnumPresentation";
            propertyPresentation->elementType = elementType;
            propertyPresentation->nameInUI = convertToUtf8("Перечисления");
            typePresentation->properties["enums"] = propertyPresentation;
        }
        presentation->addType(typePresentation);
    }

    return presentation;
}
}

std::shared_ptr<Presentation> presentationForPresentationView()
{
    return createHardcodedPresentationForPresentationView();
}

std::shared_ptr<Presentation> presentationForDesignView()
{
    // ToDo. Temp stub:
    return presentationForPresentationView();
}

void setPresentationForDesignView(const std::shared_ptr<Presentation>& presentation)
{
    // ToDO
}

} }
