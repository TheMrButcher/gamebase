/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "TypePresentation.h"
#include "EnumPresentation.h"

namespace gamebase { namespace editor {

static const char* EMPTY_TYPE_NAME = "_empty_type_name";

class Presentation : public impl::ISerializable {
public:
    Presentation(const std::string& pathToDefaultPatterns);

    void addEnum(const std::shared_ptr<EnumPresentation>& enumPresentation);
    void addType(const std::shared_ptr<TypePresentation>& typePresentation);
    void setPropertyBaseType(
        const std::string& typeName,
        const std::string& propertyName,
        const std::string& newBaseType);

    const EnumPresentation* enumByName(const std::string& name) const;
    const TypePresentation* typeByName(const std::string& name) const;
    std::vector<const TypePresentation*> derivedTypesByBaseTypeName(
        const std::string& name, bool excludeAbstract = true) const;

    std::string pathToPattern(const std::string& typeName) const;
    std::shared_ptr<IObject> loadPattern(
		const std::string& typeName, bool needSwitchReg = true) const;
    void serializeDefaultPattern(const std::string& typeName) const;
    void serializeAllDefaultPatterns() const;

    std::vector<std::string> baseTypesByTypeName(const std::string& typeName) const;

    const IPropertyPresentation* propertyByName(
        const std::string& typeName, const std::string& name);

    virtual void serialize(impl::Serializer& serializer) const override;

private:
    void addDerivedTypes(
        std::vector<const TypePresentation*>& result,
        const std::string& name, bool excludeAbstract) const;

    void serializeObjectPattern(
        const std::string& typeName, impl::Serializer& serializer) const;

    void serializePatternOfMembers(
        const std::string& typeName, impl::Serializer& serializer) const;
    
    void baseTypesByTypeNameImpl(const std::string& typeName, std::vector<std::string>& dst) const;
    
    std::string m_pathToDefaultPatterns;

    std::vector<std::shared_ptr<EnumPresentation>> m_enums;
    std::vector<std::shared_ptr<TypePresentation>> m_types;
    
    std::map<std::string, std::shared_ptr<EnumPresentation>> m_enumMap;
    std::map<std::string, std::shared_ptr<TypePresentation>> m_typeMap;
    std::map<std::string, std::vector<std::shared_ptr<TypePresentation>>> m_derivedTypesMap;
};

std::shared_ptr<Presentation> presentationForPresentationView();
std::shared_ptr<Presentation> presentationForDesignView();
void setPresentationForDesignView(const std::shared_ptr<Presentation>& presentation);

} }
