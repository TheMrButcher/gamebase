#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/reg/PropertiesRegister.h>
#include <string>

namespace gamebase { namespace impl {

class PropertiesRegisterBuilder;

class IRegistrable : public virtual IObject {
public:
    virtual void setName(const std::string& name) = 0;
    
    virtual std::string name() const = 0;

    virtual void registerObject(PropertiesRegisterBuilder* builder) = 0;

    virtual const PropertiesRegister& properties() const
    {
        return const_cast<IRegistrable*>(this)->properties();
    }

    virtual PropertiesRegister& properties() = 0;

    bool hasProperty(const std::string& name) const { return properties().hasProperty(name); }
    bool hasChild(const std::string& name) const { return properties().hasObject(name); }
    std::shared_ptr<IValue> getAbstractProperty(const std::string& name) const
    {
        return properties().getAbstractProperty(name);
    }
    IObject* tryGetAbstractChild(const std::string& name) const
    {
        return properties().tryGetAbstractObject(name);
    }
    IObject* getAbstractChild(const std::string& name) const
    {
        return properties().getAbstractObject(name);
    }

    template <typename PropertyType>
    std::shared_ptr<Value<PropertyType>> getProperty(const std::string& name) const
    {
        return properties().getProperty<PropertyType>(name);
    }

    template <typename ObjectType>
    ObjectType* getChild(const std::string& name) const
    {
        return properties().getObject<ObjectType>(name);
    }
};

} }
