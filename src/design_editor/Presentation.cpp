/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "Presentation.h"
#include "tools.h"
#include <reg/RegisterSwitcher.h>
#include <gamebase/impl/serial/JsonSerializer.h>
#include <gamebase/impl/serial/JsonDeserializer.h>

namespace gamebase { namespace editor {

namespace {
const std::string PRES_PRESENTATION_PATH = "PresPresentation.json";
const std::string PRESENTATION_PATTERNS_PATH = "PresentationPatterns";
std::shared_ptr<Presentation> PRES_PRESENTATION;

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

void Presentation::setPropertyBaseType(
    const std::string& typeName, const std::string& propertyName, const std::string& newBaseType)
{
    auto itType = m_typeMap.find(typeName);
    if (itType == m_typeMap.end()) {
        std::cout << "Can't find type by name: " << typeName << std::endl;
        return;
    }
    auto& typePresentation = itType->second;

    auto itProperty = typePresentation->properties.find(propertyName);
    if (itProperty == typePresentation->properties.end()) {
        std::cout << "Can't find property by name: " << propertyName << std::endl;
        return;
    }

    if (auto objectPresentation = dynamic_cast<ObjectPresentation*>(itProperty->second.get()))
        objectPresentation->baseType = newBaseType.empty()
            ? objectPresentation->originalBaseType : newBaseType;
    else if (auto arrayPresentation = dynamic_cast<ArrayPresentation*>(itProperty->second.get())) {
        if (auto objectPresentation = dynamic_cast<ObjectPresentation*>(arrayPresentation->elementType.get()))
            objectPresentation->baseType = newBaseType.empty()
                ? objectPresentation->originalBaseType : newBaseType;
        else
            std::cout << propertyName << " is array and has non-object elements" << std::endl;
    }
    else if (auto mapPresentation = dynamic_cast<MapPresentation*>(itProperty->second.get())) {
        if (auto objectPresentation = dynamic_cast<ObjectPresentation*>(mapPresentation->valueType.get()))
            objectPresentation->baseType = newBaseType.empty()
                ? objectPresentation->originalBaseType : newBaseType;
        else
            std::cout << propertyName << " is map and has non-object values" << std::endl;
    }
    else {
        std::cout << propertyName << " has wrong type: " << typeid(*itProperty->second).name();
    }
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
    if (!typePresentation)
        return "";
    std::string path;
    if (typePresentation->pathToPatternValue.empty()) {
        path = impl::pathToDesign(makePathStr(m_pathToDefaultPatterns, patternFileName(typeName), "json"));
    } else {
        if (isAbsolutePath(typePresentation->pathToPatternValue))
            path = typePresentation->pathToPatternValue;
        else
            path = impl::pathToDesign(typePresentation->pathToPatternValue);
    }
    return path;
}

std::shared_ptr<impl::IObject> Presentation::loadPattern(
	const std::string& typeName, bool needSwitchReg) const
{
    std::string path = pathToPattern(typeName);
    if (path.empty())
        return nullptr;
    std::shared_ptr<IObject> result;
    try {
		std::unique_ptr<RegisterSwitcher> switcher;
		if (needSwitchReg)
			switcher.reset(new RegisterSwitcher);
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
    std::string path = impl::pathToDesign(makePathStr(
        m_pathToDefaultPatterns, patternFileName(typeName), "json"));

    try {
        impl::JsonSerializer jsonSerializer;
        jsonSerializer.startObject("");

        impl::Serializer serializer(&jsonSerializer, impl::SerializationMode::Compressed);
        serializeObjectPattern(typeName, serializer);

        jsonSerializer.finishObject();
    
        std::ofstream patternFile(path);
        patternFile << jsonSerializer.toString(impl::JsonFormat::Fast);
    } catch (std::exception& ex) {
        std::cout << "Error while serializing default pattern for type: " << typeName
            << ". Reason: " << ex.what() << std::endl;
    }
}

void Presentation::serializeObjectPattern(
    const std::string& typeName, impl::Serializer& serializer) const
{
    serializer << impl::TYPE_NAME_TAG << typeName;
    serializer << impl::EMPTY_TAG << false;
    serializePatternOfMembers(typeName, serializer);
    auto baseTypeNames = baseTypesByTypeName(typeName);
    for (auto it = baseTypeNames.begin(); it != baseTypeNames.end(); ++it)
        serializePatternOfMembers(*it, serializer);
}

void Presentation::serializePatternOfMembers(
    const std::string& typeName, impl::Serializer& serializer) const
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
                        if (it->first == impl::VISIBLE_TAG)
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

			case PropertyPresentation::SpecialString:
			{
				auto specialStringType = dynamic_cast<const SpecialStringPresentation*>(
                    it->second.get())->type;
				switch (specialStringType) {
				case SpecialString::Font: vs << impl::fontStorage().defaultFamilyNameBFF(); break;
                case SpecialString::ImagePath: vs << std::string(); break;
                case SpecialString::SoundPath: vs << std::string(); break;
                default: THROW_EX() << "Unknown special string type: " << static_cast<int>(specialStringType);
				}
            } break;

            case PropertyPresentation::PrimitiveArray:
            {
                auto primitiveArrayType = dynamic_cast<const PrimitiveArrayPresentation*>(
                    it->second.get())->type;
                switch (primitiveArrayType) {
                    case impl::SerializationTag::Vec2:        vs << Vec2();        break;
                    case impl::SerializationTag::Matrix2:     vs << Matrix2();     break;
                    case impl::SerializationTag::Transform2:  vs << Transform2();  break;
                    case impl::SerializationTag::BoundingBox: vs << impl::BoundingBox(Vec2(0, 0), Vec2(0, 0)); break;
					case impl::SerializationTag::GLColor:     vs << impl::GLColor();       break;
                    default: THROW_EX() << "Unknown primitive array type: " << static_cast<int>(primitiveArrayType);
                }
            } break;

            case PropertyPresentation::Object:
            {
                auto objectPresentation = dynamic_cast<const ObjectPresentation*>(it->second.get());
                bool serializeEmptyObject = true;
                if (!objectPresentation->canBeEmpty) {
                    auto type = typeByName(objectPresentation->baseType);
                    if (!type)
                        THROW_EX() << "Can't find base type: " << objectPresentation->baseType;
                    if (!type->pathToPatternValue.empty()) {
                        auto obj = loadPattern(objectPresentation->baseType, false);
                        vs << obj;
                        serializeEmptyObject = false;
                    } else {
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

void Presentation::serialize(impl::Serializer& s) const
{
    s << "pathToDefaultPatterns" << m_pathToDefaultPatterns << "types" << m_types << "enums" << m_enums;
}

std::unique_ptr<impl::IObject> deserializePresentation(impl::Deserializer& deserializer)
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
std::shared_ptr<Presentation> loadPresentation(
    const std::string& name, const std::string& path, const std::string& defaultPathToPatterns)
{
    std::shared_ptr<Presentation> presentation;
    auto presentationPath = impl::pathToDesign(path);
    try {
        impl::deserializeFromJsonFile(presentationPath, presentation);
    } catch (std::exception& ex) {
        std::cout << "Can't load presentation '" << name << "' from: " << path
            << ". Reason: " << ex.what() << std::endl;
        presentation.reset();
    }
    if (presentation) {
        std::cout << "Successfully loaded presentation '" << name << "' from: " << path << std::endl;
    } else {
        presentation.reset(new Presentation(defaultPathToPatterns));
    }
    return presentation;
}
}

std::shared_ptr<Presentation> presentationForPresentationView()
{
    if (!PRES_PRESENTATION)
        PRES_PRESENTATION = loadPresentation("for presentation", PRES_PRESENTATION_PATH, PRESENTATION_PATTERNS_PATH);
    return PRES_PRESENTATION;
}

std::shared_ptr<Presentation> presentationForDesignView()
{
    if (!DESIGN_PRESENTATION)
        DESIGN_PRESENTATION = loadPresentation("for design", DESIGN_PRESENTATION_PATH, DESIGN_PATTERNS_PATH);
    return DESIGN_PRESENTATION;
}

void setPresentationForDesignView(const std::shared_ptr<Presentation>& presentation)
{
	auto path = impl::pathToDesign(DESIGN_PRESENTATION_PATH);
    createBackup(path, 3);
    serializeToJsonFile(presentation, impl::SerializationMode::Default, path);
    DESIGN_PRESENTATION = presentation;
}

} }
