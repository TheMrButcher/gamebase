#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IValue.h>
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

    bool has(const std::string& name) const;
    std::shared_ptr<IValue> getProperty(const std::string& name) const;
    IObject* getObject(const std::string& name) const;

    template <typename PropertyType>
    std::shared_ptr<PropertyType> getProperty(const std::string& name)
    {
        auto result = std::dynamic_pointer_cast<PropertyType>(getProperty(name));
        if (!result)
            THROW_EX() << "Type of property " << name << " differs from required";
        return result;
    }

    template <typename ObjectType>
    ObjectType* getObject(const std::string& name)
    {
        auto* result = dynamic_cast<ObjectType*>(getObject(name));
        if (!result)
            THROW_EX() << "Type of object " << name << " differs from required";
        return result;
    }

    //void remove(const std::string& name);

private:
    PropertiesRegister* findHolder(const ObjectTreePath& path);
    const PropertiesRegister* findHolder(const ObjectTreePath& path) const;

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
