#pragma once

#include "TypePresentation.h"
#include "EnumPresentation.h"

namespace gamebase { namespace editor {

static const char* EMPTY_TYPE_NAME = "_empty_type_name";

class Presentation : public ISerializable {
public:
    void addEnum(const std::shared_ptr<EnumPresentation>& enumPresentation);
    void addType(const std::shared_ptr<TypePresentation>& typePresentation);

    const EnumPresentation* enumByName(const std::string& name) const;
    const TypePresentation* typeByName(const std::string& name) const;
    std::vector<const TypePresentation*> derivedTypesByBaseTypeName(
        const std::string& name, bool excludeAbstract = true) const;

    /*template <typename PropertyPresentationType>
    std::shared_ptr<PropertyPresentationType> propertyByName(
        const std::string& typeName, const std::string& name)
    {
        auto propertyPresentation = abstractPropertyByName(typeName, name);
        if (!propertyPresentation) {
            std::cout << "Can't find property by name: " << typeName << "::" << name << std::endl;
            return nullptr;
        }
        auto result = std::dynamic_pointer_cast<PropertyPresentationType>(propertyPresentation);
        if (!result)
            THROW_EX() << "Can't cast property presentation of '" << typeName << "::" << name << "' of type " <<
                typeid(*it->second).name() << " to type " << typeid(PropertyPresentationType).name();
        return result;
    }*/

    const IPropertyPresentation* propertyByName(
        const std::string& typeName, const std::string& name);

    virtual void serialize(Serializer& serializer) const override;

private:
    void addDerivedTypes(
        std::vector<const TypePresentation*>& result,
        const std::string& name, bool excludeAbstract) const;

    std::vector<std::shared_ptr<EnumPresentation>> m_enums;
    std::vector<std::shared_ptr<TypePresentation>> m_types;
    
    std::map<std::string, std::shared_ptr<EnumPresentation>> m_enumMap;
    std::map<std::string, std::shared_ptr<TypePresentation>> m_typeMap;
    std::map<std::string, std::vector<std::shared_ptr<TypePresentation>>> m_derivedTypesMap;
};

std::shared_ptr<Presentation> presentationForPresentationView();
std::shared_ptr<Presentation> presentationForDesignView();
void setPresentationForDesignView(const std::shared_ptr<Presentation>& presentation);
void generatePresentationPatternsForPresentationView();

} }
