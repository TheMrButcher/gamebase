/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "PropertyPresentation.h"
#include <gamebase/tools/Exception.h>
#include <vector>
#include <map>

namespace gamebase { namespace editor {

class TypePresentation : public impl::ISerializable {
public:
    TypePresentation() : isAbstract(false) {}

    template <typename PropertyPresentationType>
    std::shared_ptr<PropertyPresentationType> propertyByName(const std::string& name) const
    {
        auto it = properties.find(name);
        if (it == properties.end()) {
            std::cout << "Can't find property by name: " << name << std::endl;
            return nullptr;
        }
        auto result = std::dynamic_pointer_cast<PropertyPresentationType>(it->second);
        if (!result)
            THROW_EX() << "Can't cast property '" << name << "' of type " <<
                typeid(*it->second).name() << " to type " << typeid(PropertyPresentationType).name();
        return result;
    }

    virtual void serialize(impl::Serializer& serializer) const override;

    bool isAbstract;
    std::string name;
    std::string nameInUI;
    std::vector<std::string> parents;
    std::map<std::string, std::shared_ptr<IPropertyPresentation>> properties;
    std::string pathToPatternValue;
};

} }
