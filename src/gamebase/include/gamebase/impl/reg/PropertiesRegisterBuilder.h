/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/reg/IRegistrable.h>
#include <gamebase/impl/reg/ValueLink.h>
#include <gamebase/impl/reg/ValueNotifyingLink.h>
#include <gamebase/impl/reg/ValueLinkWithSetter.h>
#include <gamebase/math/Transform2.h>
#include <gamebase/impl/geom/BoundingBox.h>
#include <gamebase/impl/graphics/GLColor.h>
#include <functional>

namespace gamebase { namespace impl {

class IObject;

class GAMEBASE_API PropertiesRegisterBuilder {
public:
    PropertiesRegisterBuilder();
    
    void registerObject(IObject* obj);
    void registerObject(const std::shared_ptr<IObject>& obj) { registerObject(obj.get()); }
    void registerObject(IObject& obj) { registerObject(&obj); }
    void registerObject(const std::string& name, IObject* obj);
    
    template <typename PropertyType>
    void registerProperty(
        const std::string& name,
        PropertyType* prop,
        const std::function<void()>& notifier = nullptr)
    {
        if (notifier) {
            m_current->properties().add(
                name,
                std::make_shared<ValueNotifyingLink<PropertyType>>(prop, notifier));
        } else {
            m_current->properties().add(
                name,
                std::make_shared<ValueLink<PropertyType>>(prop));
        }
    }

    template <typename PropertyType, typename SetterType>
    void registerPropertyWithSetter(
        const std::string& name,
        PropertyType* prop,
        const SetterType& setter)
    {
		std::function<void(const PropertyType&)> setterFunc(setter);
        m_current->properties().add(
            name,
            std::make_shared<ValueLinkWithSetter<PropertyType>>(prop, setterFunc));
    }

    void registerColor(
        const std::string& name,
        GLColor* prop,
        const std::function<void()>& notifier = nullptr);

    void registerVec2(
        const std::string& name,
        Vec2* prop,
        const std::function<void()>& notifier = nullptr);

private:
    PropertiesRegisterBuilder(IRegistrable* current);

    void registerInCurrent(const std::string& name, IObject* obj);
    void buildRegister(const std::string& regName, IRegistrable* registrable);

    IRegistrable* m_current;
};

extern GAMEBASE_API PropertiesRegisterBuilder g_registryBuilder;

} }
