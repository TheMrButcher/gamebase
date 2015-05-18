#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/serial/ISerializable.h>
#include <gamebase/utils/Exception.h>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <iostream>

namespace gamebase {

class Deserializer;

class GAMEBASE_API SerializableRegister {
public:
    static SerializableRegister& instance()
    {
        static SerializableRegister reg;
        return reg;
    }

    template <typename T>
    void registerType(
        const std::string& typeName,
        const std::function<IObject*(Deserializer&)>& deserialize)
    {
        std::type_index typeIndex = typeid(T);
        m_nameToType.insert(std::make_pair(
            typeName, TypeTraits(typeIndex, deserialize)));
        m_typeToName[typeIndex] = typeName;
    }

    struct TypeTraits {
        TypeTraits(
            const std::type_index& index,
            const std::function<IObject*(Deserializer&)>& deserialize)
            : index(index)
            , deserialize(deserialize)
        {}

        std::type_index index;
        std::function<IObject*(Deserializer&)> deserialize;
    };

    bool isRegistered(const std::string& name) const
    {
        return m_nameToType.count(name) > 0;
    }

    template <typename T>
    bool isRegistered() const
    {
        return isRegistered(typeid(T));
    }

    bool isRegistered(const std::type_info& typeInfo) const
    {
        return isRegistered(std::type_index(typeInfo));
    }

    bool isRegistered(const std::type_index& typeIndex) const
    {
        return m_typeToName.count(typeIndex) > 0;
    }

    const TypeTraits& typeTraits(const std::string& name) const
    {
        auto it = m_nameToType.find(name);
        if (it == m_nameToType.end())
            THROW_EX() << "Type " << name << " is not registered";
        return it->second;
    }

    template <typename T>
    const std::string& typeName() const
    {
        return typeName(typeid(T));
    }

    const std::string& typeName(const std::type_info& typeInfo) const
    {
        return typeName(std::type_index(typeInfo));
    }
    
    const std::string& typeName(const std::type_index& typeIndex) const
    {
        auto it = m_typeToName.find(typeIndex);
        if (it == m_typeToName.end())
            THROW_EX() << "Type with index " << typeIndex.name() << " is not registered.";
        return it->second;
    }

    void debugPrint() const
    {
        std::cout << "Register ID: " << reinterpret_cast<unsigned int>(this) << std::endl;
        std::cout << "Registered classes: " << m_nameToType.size() << std::endl;
        for (auto it = m_nameToType.begin(); it != m_nameToType.end(); ++it)
            std::cout << it->first << " (type_index: " << it->second.index.name() << ")" << std::endl;
    }

private:
    SerializableRegister() {}

    std::unordered_map<std::string, TypeTraits> m_nameToType;
    std::unordered_map<std::type_index, std::string> m_typeToName;
};

#define REGISTER_TEMPLATE(className, argName) \
    namespace { \
    class _GamebaseRegister##className##argName { \
    public: \
        _GamebaseRegister##className##argName() \
        { \
            SerializableRegister::instance().registerType<className<argName>>( \
                #className"<"#argName">", &deserialize##className<argName>); \
        } \
    } _gamebaseRegister##className##argName; \
    }

#define REGISTER_CLASS2(className, deserializerName) \
    namespace { \
    class _GamebaseRegister##className { \
    public: \
        _GamebaseRegister##className() \
        { \
            SerializableRegister::instance().registerType<className>(#className, deserializerName); \
        } \
    } _gamebaseRegister##className; \
    }

#define REGISTER_CLASS(className) REGISTER_CLASS2(className, deserialize##className)
    

}
