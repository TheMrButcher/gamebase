#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/reg/Value.h>
#include <gamebase/impl/engine/IObject.h>
#include <gamebase/tools/Exception.h>
#include <string>
#include <memory>
#include <map>
#include <unordered_set>

namespace gamebase { namespace impl {

class PropertiesRegisterBuilder;
class IRegistrable;
struct ObjectTreePath;

class GAMEBASE_API PropertiesRegister {
public:
    PropertiesRegister();

    void setName(const std::string& name) { m_name = name; }
    const std::string& name() const { return m_name; }

    bool hasProperty(const std::string& name) const;
    bool hasObject(const std::string& name) const;
    std::shared_ptr<IValue> getAbstractProperty(const std::string& name) const;
    IObject* tryGetAbstractObject(const std::string& name) const;
    IObject* getAbstractObject(const std::string& name) const;

    template <typename PropertyType>
    std::shared_ptr<Value<PropertyType>> getProperty(const std::string& name) const
    {
        auto abstractProperty = getAbstractProperty(name);
        if (!abstractProperty)
            THROW_EX() << "Registry doesn't contain property " << name;
        auto result = std::dynamic_pointer_cast<Value<PropertyType>>(abstractProperty);
        if (!result)
            THROW_EX() << "Type of property " << name << " differs from required: " << typeid(PropertyType).name();
        return result;
    }

    template <typename ObjectType>
    ObjectType* getObject(const std::string& name) const
    {
        auto abstractObject = getAbstractObject(name);
        if (!abstractObject)
            THROW_EX() << "Registry doesn't contain object " << name;
        auto* result = dynamic_cast<ObjectType*>(abstractObject);
        if (!result)
            THROW_EX() << "Type (" << typeid(*abstractObject).name() << ") of object " << name
                << " differs from required: " << typeid(ObjectType).name();
        return result;
    }

    bool empty() const
    {
        return m_properties.empty() && m_objects.empty() && m_anonObjects.empty();
    }

    void clear()
    {
        m_properties.clear();
        m_objects.clear();
    }

    IRegistrable* holder() const
    {
        return m_current;
    }

    IRegistrable* parent() const
    {
        return m_parent;
    }

    template <typename ObjectType>
    ObjectType* findParentOfType() const
    {
        auto* parent = m_parent;
        if (!parent)
            THROW_EX() << "Can't find any parent of object, it has no parents";
        while (parent) {
            auto* castedParent = dynamic_cast<ObjectType*>(parent);
            if (castedParent)
                return castedParent;
            parent = parent->properties().parent();
        }
        THROW_EX() << "Can't find parent of required type: " << typeid(ObjectType).name();
    }

    void remove(const std::string& name);
    void remove(IObject* obj);

private:
    std::pair<PropertiesRegister*, PropertiesRegister*> find(const ObjectTreePath& path);
    std::pair<const PropertiesRegister*, const PropertiesRegister*> find(const ObjectTreePath& path) const;

    friend class PropertiesRegisterBuilder;

    void add(const std::string& name, const std::shared_ptr<IValue>& value);
    void add(IObject* obj);
    void add(const std::string& name, IObject* obj);

    std::string m_name;
    IRegistrable* m_current;
    IRegistrable* m_parent;

    struct NamedProperty {
        NamedProperty() {}
        NamedProperty(const std::string& name, const std::shared_ptr<IValue>& prop)
            : name(name), prop(prop)
        {}

        std::string name;
        std::shared_ptr<IValue> prop;
    };

    struct NamedObject {
        NamedObject() {}
        NamedObject(const std::string& name, IObject* obj)
            : name(name), obj(obj)
        {}

        std::string name;
        IObject* obj;
    };

    std::vector<NamedProperty> m_properties;
    std::vector<NamedObject> m_objects;
    std::unordered_set<IObject*> m_anonObjects;
};

} }
