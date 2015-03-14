#include <stdafx.h>
#include <gamebase/engine/PropertiesRegister.h>
#include <gamebase/engine/IRegistrable.h>
#include <vector>
#include <sstream>

namespace gamebase {

PropertiesRegister::PropertiesRegister()
    : m_current(nullptr)
    , m_parent(nullptr)
{}

std::string PropertiesRegister::fullName() const
{
    std::vector<std::string> names;
    names.push_back(m_registrableName);
    IRegistrable* registrable = m_parent;
    if (registrable == nullptr)
        return "/";
    while (true) {
        auto& props = registrable->properties();
        if (props.m_parent == nullptr)
            break;
        names.push_back(props.m_registrableName);
        registrable = props.m_parent;
    }
    std::ostringstream oss;
    for (auto it = names.rbegin(); it != names.rend(); ++it)
        oss << '/' << *it;
    return oss.str();
}

struct ObjectTreePath {
    ObjectTreePath(const std::string& pathStr)
        : pathStr(pathStr)
    {
        if (pathStr.empty())
            THROW_EX() << "Can't build ObjectTreePath, empty string";
        isAbsolute = pathStr[0] == '/';
        size_t index = 0;
        size_t next;
        while ((next = pathStr.find('/', index)) != pathStr.npos) {
            if (next != index)
                path.push_back(pathStr.substr(index, next - index));
            index = next + 1;
        }
        if (index < pathStr.length())
            path.push_back(pathStr.substr(index));
    }

    bool isAbsolute;
    std::vector<std::string> path;
    std::string pathStr;
};

bool PropertiesRegister::has(const std::string& name) const
{
    ObjectTreePath path(name);
    if (path.path.empty())
        return path.isAbsolute;
    auto* props = findHolder(path);
    if (props) {
        if (props->m_properties.find(path.path.back()) != props->m_properties.end())
            return true;
        if (props->m_objects.find(path.path.back()) != props->m_objects.end())
            return true;
    }
    return false;
}

std::shared_ptr<IValue> PropertiesRegister::getProperty(const std::string& name) const
{
    ObjectTreePath path(name);
    if (path.path.empty())
        return false;
    auto* props = findHolder(path);
    if (!props)
        THROW_EX() << "Can't find object holding property, name: " << name;
    auto it = props->m_properties.find(path.path.back());
    if (it == props->m_properties.end())
        THROW_EX() << "Can't find property, name: " << name;
    return it->second;
}

IObject* PropertiesRegister::getObject(const std::string& name) const
{
    ObjectTreePath path(name);
    if (auto* props = findHolder(path)) {
        if (path.path.empty())
            return props->m_current;
        auto it = props->m_objects.find(path.path.back());
        if (it != props->m_objects.end())
            return it->second;
    }
    THROW_EX() << "Can't find object, name: " << name;
}

PropertiesRegister* PropertiesRegister::findHolder(const ObjectTreePath& path)
{
    if (!path.isAbsolute && path.path.empty())
        return nullptr;

    IRegistrable* cur = m_current;
    if (path.isAbsolute) {
        while (cur->properties().m_parent != nullptr)
            cur = cur->properties().m_parent;
    }

    for (auto pathIt = path.path.begin(); pathIt != path.path.end(); ++pathIt) {
        const auto& pathElem = *pathIt;
        auto& props = cur->properties();
        if (pathElem == ".")
            continue;
        if (pathElem == "..") {
            auto* parent = props.m_parent;
            if (parent == nullptr)
                return nullptr;
            cur = parent;
            continue;
        }

        {
            auto it = props.m_objects.find(pathElem);
            if (it != props.m_objects.end()) {
                auto* registrable = dynamic_cast<IRegistrable*>(it->second);
                if (!registrable) {
                    if (pathIt + 1 != path.path.end())
                        return nullptr;
                    return &props;
                }
                cur = registrable;
                continue;
            }
        }

        {
            auto it = props.m_properties.find(pathElem);
            if (it != props.m_properties.end()) {
                if (pathIt + 1 != path.path.end())
                    return nullptr;
                return &props;
            }
        }

        return nullptr;
    }
        
    auto& props = cur->properties();
    if (props.m_parent == nullptr)
        return &props;
    return &props.m_parent->properties();
}

const PropertiesRegister* PropertiesRegister::findHolder(const ObjectTreePath& path) const
{
    return const_cast<PropertiesRegister*>(this)->findHolder(path);
}

void PropertiesRegister::add(
    const std::string& name,
    const std::shared_ptr<IValue>& value)
{
    if (name.empty())
        THROW_EX() << "Can't register anonymous property";
    auto it = m_properties.find(name);
    if (it != m_properties.end())
        THROW_EX() << "Already registered property, name: " << name;
    m_properties[name] = value;
}

void PropertiesRegister::add(
    const std::string& name,
    IObject* obj)
{
    if (name.empty())
        THROW_EX() << "Can't register object without name, anonymous objects should get registrable ID";
    auto it = m_objects.find(name);
    if (it != m_objects.end())
        THROW_EX() << "Already registered object, name: " << name;
    m_objects[name] = obj;
}
}
