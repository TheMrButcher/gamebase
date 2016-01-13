#include "Presentation.h"
#include "tools.h"
#include <gamebase/serial/JsonSerializer.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <gamebase/text/Conversion.h>

namespace gamebase { namespace editor {

namespace {
const std::string PRESENTATION_PATTERNS_PATH = "PresentationPatterns";
const std::string DESIGN_PRESENTATION_PATH = "DesignPresentation.json";
const std::string DESIGN_PATTERNS_PATH = "DesignPatterns";
std::shared_ptr<Presentation> DESIGN_PRESENTATION;

std::string patternFileName(const std::string& typeName)
{
    std::string result(typeName);
    size_t i = 0;
    while ((i = result.find('<')) != std::string::npos)
        result[i] = '(';
    while ((i = result.find('>')) != std::string::npos)
        result[i] = ')';
    return result;
}
}

Presentation::Presentation(const std::string& pathToDefaultPatterns)
    : m_pathToDefaultPatterns(pathToDefaultPatterns)
{
    if (m_pathToDefaultPatterns.empty()) {
        std::cout << "Warning: path to patterns is empty" << std::endl;
    } else {
        auto fileDesc = fileInfo(m_pathToDefaultPatterns);
        if (fileDesc.type != FileDesc::Directory) {
            if (fileDesc.type != FileDesc::None)
                removeFile(m_pathToDefaultPatterns);
            createDir(m_pathToDefaultPatterns);
            std::cout << "Created directory for presentation: " << m_pathToDefaultPatterns << std::endl;
        }
    }
}

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

const EnumPresentation* Presentation::enumByName(const std::string& name) const
{
    auto it = m_enumMap.find(name);
    if (it == m_enumMap.end()) {
        std::cout << "Can't find enum by name: " << name << std::endl;
        return nullptr;
    }
    return it->second.get();
}

const TypePresentation* Presentation::typeByName(const std::string& name) const
{
    if (name == EMPTY_TYPE_NAME)
        return nullptr;
    auto it = m_typeMap.find(name);
    if (it == m_typeMap.end()) {
        std::cout << "Can't find type by name: " << name << std::endl;
        return nullptr;
    }
    return it->second.get();
}

std::vector<const TypePresentation*> Presentation::derivedTypesByBaseTypeName(
    const std::string& name, bool excludeAbstract) const
{
    std::vector<const TypePresentation*> result;
    if (auto typePresentation = typeByName(name)) {
        if (!excludeAbstract || !typePresentation->isAbstract)
            result.push_back(typePresentation);
    }

    addDerivedTypes(result, name, excludeAbstract);
    return std::move(result);
}

std::string Presentation::pathToPattern(const std::string& typeName) const
{
    auto* typePresentation = typeByName(typeName);
    std::string path;
    if (typePresentation->pathToPatternValue.empty()) {
        path = pathToDesign(makePathStr(m_pathToDefaultPatterns, patternFileName(typeName), "json"));
    } else {
        if (isAbsolutePath(typePresentation->pathToPatternValue))
            path = typePresentation->pathToPatternValue;
        else
            path = pathToDesign(typePresentation->pathToPatternValue);
    }
    return path;
}

std::shared_ptr<IObject> Presentation::loadPattern(const std::string& typeName) const
{
    auto* typePresentation = typeByName(typeName);
    if (typePresentation->isAbstract)
        return nullptr;
    std::string path = pathToPattern(typeName);
    std::shared_ptr<IObject> result;
    try {
        deserializeFromJsonFile(path, result);
    } catch (std::exception& ex) {
        std::cerr << "Error while deserializing object of type " << typeName << " from file " << path
            << ". Reason: " << ex.what() << std::endl;
        result.reset();
    }
    return result;
}

void Presentation::serializeDefaultPattern(const std::string& typeName) const
{
    auto* typePresentation = typeByName(typeName);
    if (!typePresentation)
        return;
    if (typePresentation->isAbstract)
        return;
    std::string path = pathToDesign(makePathStr(
        m_pathToDefaultPatterns, patternFileName(typeName), "json"));

    try {
        JsonSerializer jsonSerializer;
        jsonSerializer.startObject("");

        Serializer serializer(&jsonSerializer);
        serializeObjectPattern(typeName, serializer);

        jsonSerializer.finishObject();
    
        std::ofstream patternFile(path);
        patternFile << jsonSerializer.toString(JsonFormat::Fast);
    } catch (std::exception& ex) {
        std::cout << "Error while serializing default pattern for type: " << typeName
            << ". Reason: " << ex.what() << std::endl;
    }
}

void Presentation::serializeObjectPattern(
    const std::string& typeName, Serializer& serializer) const
{
    serializer << TYPE_NAME_TAG << typeName;
    serializer << EMPTY_TAG << false;
    serializePatternOfMembers(typeName, serializer);
    auto baseTypeNames = baseTypesByTypeName(typeName);
    for (auto it = baseTypeNames.begin(); it != baseTypeNames.end(); ++it)
        serializePatternOfMembers(*it, serializer);
}

void Presentation::serializePatternOfMembers(
    const std::string& typeName, Serializer& serializer) const
{
    auto* typePresentation = typeByName(typeName);
    if (!typePresentation)
        return;
    for (auto it = typePresentation->properties.begin(); it != typePresentation->properties.end(); ++it) {
        auto propertyPresentationType = it->second->presentationType();
        auto vs = (serializer << it->first);
        switch (propertyPresentationType) {
            case PropertyPresentation::Primitive:
            {
                auto primitiveType = dynamic_cast<const PrimitivePropertyPresentation*>(
                    it->second.get())->type;
                switch (primitiveType) {
                    case PrimitiveType::Float:  vs << 0.0f;            break;
                    case PrimitiveType::Double: vs << 0.0;             break;
                    case PrimitiveType::Int:    vs << int(0);          break;
                    case PrimitiveType::UInt:   vs << unsigned int(0); break;
                    case PrimitiveType::Int64:  vs << int64_t(0);      break;
                    case PrimitiveType::UInt64: vs << uint64_t(0);     break;
                    case PrimitiveType::Bool:
                        if (it->first == VISIBLE_TAG)
                            vs << true;
                        else
                            vs << false;
                        break;
                    case PrimitiveType::String: vs << std::string();   break;
                    default: THROW_EX() << "Unknown primitive type: " << static_cast<int>(primitiveType);
                }
            } break;

            case PropertyPresentation::Enum:
            {
                auto enumTypeName = dynamic_cast<const EnumPropertyPresentation*>(
                    it->second.get())->type;
                auto* enumPresentation = enumByName(enumTypeName);
                if (enumPresentation && !enumPresentation->values.empty())
                    vs << enumPresentation->values.begin()->first;
                else
                    vs << int(0);
            } break;

            case PropertyPresentation::PrimitiveArray:
            {
                auto primitiveArrayType = dynamic_cast<const PrimitiveArrayPresentation*>(
                    it->second.get())->type;
                switch (primitiveArrayType) {
                    case SerializationTag::Vec2:        vs << Vec2();        break;
                    case SerializationTag::Matrix2:     vs << Matrix2();     break;
                    case SerializationTag::Transform2:  vs << Transform2();  break;
                    case SerializationTag::BoundingBox: vs << BoundingBox(Vec2(0, 0), Vec2(0, 0)); break;
                    case SerializationTag::Color:       vs << Color();       break;
                    default: THROW_EX() << "Unknown primitive array type: " << static_cast<int>(primitiveArrayType);
                }
            } break;

            case PropertyPresentation::Object:
            {
                auto objectPresentation = dynamic_cast<const ObjectPresentation*>(it->second.get());
                bool serializeEmptyObject = true;
                if (!objectPresentation->canBeEmpty) {
                    auto derivedTypes = derivedTypesByBaseTypeName(objectPresentation->baseType);
                    if (!derivedTypes.empty()) {
                        serializeEmptyObject = false;
                        auto objectTypeName = derivedTypes.front()->name;
                        try {
                            serializer.innerSerializer()->startObject(it->first);
                            serializeObjectPattern(objectTypeName, serializer);
                            serializer.innerSerializer()->finishObject();
                        } catch (std::exception& ex) {
                            THROW_EX() << "Error while serializing default pattern for type: " << objectTypeName
                                << ". Reason: " << ex.what();
                        }
                    }
                }
                if (serializeEmptyObject) {
                    std::shared_ptr<IObject> obj;
                    vs << obj;
                }
            } break;

            case PropertyPresentation::Array:
            {
                std::vector<int> arr;
                vs << arr;
            } break;

            case PropertyPresentation::Map:
            {
                std::map<int, int> m;
                vs << m;
            } break;

            default: THROW_EX() << "Unknown property presentation type: " << static_cast<int>(propertyPresentationType);
        }
    }
}

void Presentation::serializeAllDefaultPatterns() const
{
    for (auto it = m_types.begin(); it != m_types.end(); ++it)
        serializeDefaultPattern((*it)->name);
}

const IPropertyPresentation* Presentation::propertyByName(
    const std::string& typeName, const std::string& name)
{
    if (auto typePresentation = typeByName(typeName)) {
        auto it = typePresentation->properties.find(name);
        if (it != typePresentation->properties.end())
            return it->second.get();
        for (auto it = typePresentation->parents.begin(); it != typePresentation->parents.end(); ++it) {
            auto propertyFromParent = propertyByName(*it, name);
            if (propertyFromParent)
                return propertyFromParent;
        }
    }
    return nullptr;
}

std::vector<std::string> Presentation::baseTypesByTypeName(const std::string& typeName) const
{
    std::vector<std::string> result;
    result.push_back(typeName);
    baseTypesByTypeNameImpl(typeName, result);
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return std::move(result);
}

void Presentation::baseTypesByTypeNameImpl(
    const std::string& typeName, std::vector<std::string>& dst) const
{
    auto* typePresentation = typeByName(typeName);
    if (!typePresentation)
        return;
    for (auto it = typePresentation->parents.begin(); it != typePresentation->parents.end(); ++it) {
        dst.push_back(*it);
        baseTypesByTypeNameImpl(*it, dst);
    }
}

void Presentation::serialize(Serializer& s) const
{
    s << "pathToDefaultPatterns" << m_pathToDefaultPatterns << "types" << m_types << "enums" << m_enums;
}

std::unique_ptr<IObject> deserializePresentation(Deserializer& deserializer)
{
    DESERIALIZE(std::string, pathToDefaultPatterns);
    DESERIALIZE(std::vector<std::shared_ptr<TypePresentation>>, types);
    DESERIALIZE(std::vector<std::shared_ptr<EnumPresentation>>, enums);

    std::unique_ptr<Presentation> result(new Presentation(pathToDefaultPatterns));
    for (auto it = enums.begin(); it != enums.end(); ++it)
        result->addEnum(*it);
    for (auto it = types.begin(); it != types.end(); ++it)
        result->addType(*it);
    return std::move(result);
}

REGISTER_CLASS(Presentation);

void Presentation::addDerivedTypes(
    std::vector<const TypePresentation*>& result,
    const std::string& name, bool excludeAbstract) const
{
    auto it = m_derivedTypesMap.find(name);
    if (it == m_derivedTypesMap.end())
        return;
    const auto& derivedTypes = it->second;
    for (auto itDerived = derivedTypes.begin(); itDerived != derivedTypes.end(); ++itDerived) {
        const auto& typePresentation = *itDerived;
        if (!excludeAbstract || !typePresentation->isAbstract)
            result.push_back(typePresentation.get());
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
    //typePresentation->pathToPatternValue = PRESENTATION_PATTERNS_PATH + name + ".json";
    dst.addType(typePresentation);
    return typePresentation;
}

std::shared_ptr<TypePresentation> addAbstractType(
    Presentation& dst, const std::string& name, const std::string& nameInUI,
    const std::vector<std::string>& parents = std::vector<std::string>())
{
    auto typePresentation = addType(dst, name, nameInUI, parents);
    typePresentation->isAbstract = true;
    typePresentation->pathToPatternValue = "";
    return typePresentation;
}

std::shared_ptr<Presentation> createHardcodedPresentationForPresentationView()
{
    auto presentation = std::make_shared<Presentation>(PRESENTATION_PATTERNS_PATH);
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
        typePresentation->pathToPatternValue = "OverrideDesignPatterns\\TypePresentation.json";
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
        {
            auto propertyPresentation = std::make_shared<PrimitivePropertyPresentation>();
            propertyPresentation->type = PrimitiveType::String;
            propertyPresentation->nameInUI = convertToUtf8("Путь к образцу");
            typePresentation->properties["pathToPatternValue"] = propertyPresentation;
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
            keyType->type = PrimitiveType::Int;
            propertyPresentation->keyType = keyType;
            auto valueType = std::make_shared<PrimitivePropertyPresentation>();
            valueType->type = PrimitiveType::String;
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
        {
            auto propertyPresentation = std::make_shared<PrimitivePropertyPresentation>();
            propertyPresentation->type = PrimitiveType::String;
            propertyPresentation->nameInUI = convertToUtf8("Путь к паттернам по умолчанию");
            typePresentation->properties["pathToDefaultPatterns"] = propertyPresentation;
        }
    }

    return presentation;
}

std::shared_ptr<Presentation> loadPresentationForDesignView()
{
    std::shared_ptr<Presentation> presentation;
    auto presentationPath = pathToDesign(DESIGN_PRESENTATION_PATH);
    try {
        deserializeFromJsonFile(presentationPath, presentation);
    } catch (std::exception& ex) {
        std::cout << "Can't load presentation for design from: " << presentationPath
            << ". Reason: " << ex.what() << std::endl;
        presentation.reset();
    }
    if (presentation) {
        std::cout << "Successfully loaded presentation for design from: " << presentationPath << std::endl;
    } else {
        presentation.reset(new Presentation(DESIGN_PATTERNS_PATH));
    }
    return presentation;
}
}

std::shared_ptr<Presentation> presentationForPresentationView()
{
    static const std::shared_ptr<Presentation> PRESENTATION =
        createHardcodedPresentationForPresentationView();
    return PRESENTATION;
}

std::shared_ptr<Presentation> presentationForDesignView()
{
    if (!DESIGN_PRESENTATION)
        DESIGN_PRESENTATION = loadPresentationForDesignView();
    return DESIGN_PRESENTATION;
}

void setPresentationForDesignView(const std::shared_ptr<Presentation>& presentation)
{
    createBackup(DESIGN_PRESENTATION_PATH, 3);
    serializeToJsonFile(presentation, JsonFormat::Styled, DESIGN_PRESENTATION_PATH);
    DESIGN_PRESENTATION = presentation;
}

} }
