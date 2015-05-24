#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/Value.h>
#include <gamebase/engine/IObject.h>
#include <gamebase/utils/Exception.h>
#include <string>
#include <memory>
#include <map>

namespace gamebase {

class PropertiesRegisterBuilder;
class IRegistrable;
struct ObjectTreePath;

class GAMEBASE_API PropertiesRegister {
public:
    PropertiesRegister();

    void setName(const std::string& name) { m_name = name; }
    std::string name() const { return m_name; }

    std::string fullName() const;

    bool hasProperty(const std::string& name) const;
    bool hasObject(const std::string& name) const;
    std::shared_ptr<IValue> getAbstractProperty(const std::string& name) const;
    IObject* getAbstractObject(const std::string& name) const;

    template <typename PropertyType>
    std::shared_ptr<Value<PropertyType>> getProperty(const std::string& name) const
    {
        auto result = std::dynamic_pointer_cast<Value<PropertyType>>(getAbstractProperty(name));
        if (!result)
            THROW_EX() << "Type of property " << name << " differs from required";
        return result;
    }

    template <typename ObjectType>
    ObjectType* getObject(const std::string& name) const
    {
        auto* result = dynamic_cast<ObjectType*>(getAbstractObject(name));
        if (!result)
            THROW_EX() << "Type of object " << name << " differs from required";
        return result;
    }

    void clear()
    {
        m_properties.clear();
        m_objects.clear();
    }

    //void remove(const std::string& name);

private:
    std::pair<PropertiesRegister*, PropertiesRegister*> find(const ObjectTreePath& path);
    std::pair<const PropertiesRegister*, const PropertiesRegister*> find(const ObjectTreePath& path) const;

    friend class PropertiesRegisterBuilder;

    void add(
        const std::string& name,
        const std::shared_ptr<IValue>& value);
    void add(
        const std::string& name,
        IObject* obj);

    std::string m_name;
    std::string m_registrableName;
    IRegistrable* m_current;
    IRegistrable* m_parent;
    std::map<std::string, std::shared_ptr<IValue>> m_properties;
    std::map<std::string, IObject*> m_objects;
};
}
