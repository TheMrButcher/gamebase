#pragma once

#include <gamebase/engine/IRegistrable.h>
#include <gamebase/engine/ValueLink.h>
#include <gamebase/engine/ValueNotifyingLink.h>
#include <gamebase/engine/ValueLinkWithSetter.h>
#include <gamebase/math/Transform2.h>
#include <gamebase/geom/BoundingBox.h>
#include <gamebase/graphics/Color.h>
#include <functional>

namespace gamebase {

class IObject;

class GAMEBASE_API PropertiesRegisterBuilder {
public:
    PropertiesRegisterBuilder();

    std::string registrableName(const std::string& name);
    
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
    void registerProperty(
        const std::string& name,
        PropertyType* prop,
        const SetterType& setter)
    {
        m_current->properties().add(
            name,
            std::make_shared<ValueLinkWithSetter<PropertyType, SetterType>>(prop, setter));
    }

    void registerColor(
        const std::string& name,
        Color* prop,
        const std::function<void()>& notifier = nullptr);

    void registerVec2(
        const std::string& name,
        Vec2* prop,
        const std::function<void()>& notifier = nullptr);

private:
    PropertiesRegisterBuilder(
        const std::shared_ptr<unsigned int>& anonIndex,
        IRegistrable* current);

    std::string registerInCurrent(const std::string& name, IObject* obj);
    void buildRegister(const std::string& regName, IRegistrable* registrable);

    std::shared_ptr<unsigned int> m_anonymousIndex;
    IRegistrable* m_current;
};

extern GAMEBASE_API PropertiesRegisterBuilder g_registryBuilder;
}
