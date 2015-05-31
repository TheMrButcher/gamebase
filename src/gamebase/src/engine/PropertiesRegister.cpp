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

bool PropertiesRegister::hasProperty(const std::string& name) const
{
    if (name.empty())
        return false;
    ObjectTreePath path(name);
    auto parentAndNode = find(path);
    if (auto* props = parentAndNode.first) {
        if (props->m_properties.find(path.path.back()) != props->m_properties.end())
            return true;
    }
    return false;
}
    
bool PropertiesRegister::hasObject(const std::string& name) const
{
    if (name.empty())
        return false;
    ObjectTreePath path(name);
    auto parentAndNode = find(path);
    if (parentAndNode.second)
        return true;
    if (auto* props = parentAndNode.first) {
        if (props->m_objects.find(path.path.back()) != props->m_objects.end())
            return true;
    }
    return false;
}

std::shared_ptr<IValue> PropertiesRegister::getAbstractProperty(const std::string& name) const
{
    ObjectTreePath path(name);
    if (path.path.empty())
        THROW_EX() << "Can't find object that holds property, empty path";
    auto parentAndNode = find(path);
    if (parentAndNode.second)
        THROW_EX() << "Can't get property, it's object, name: " << name;

    if (auto* props = parentAndNode.first) {
        auto it = props->m_properties.find(path.path.back());
        if (it == props->m_properties.end())
            THROW_EX() << "Can't find property, name: " << name;
        return it->second;
    }
    THROW_EX() << "Can't find object that holds property, name: " << name;
}

IObject* PropertiesRegister::getAbstractObject(const std::string& name) const
{
    ObjectTreePath path(name);
    auto parentAndNode = find(path);
    if (parentAndNode.second)
        return parentAndNode.second->m_current;
    if (auto* props = parentAndNode.first) {
        auto it = props->m_objects.find(path.path.back());
        if (it != props->m_objects.end())
            return it->second;
    }
    THROW_EX() << "Can't find object, name: " << name;
}

std::pair<PropertiesRegister*, PropertiesRegister*> PropertiesRegister::find(
    const ObjectTreePath& path)
{
    if (!path.isAbsolute && path.path.empty())
        return std::make_pair(nullptr, nullptr);

    IRegistrable* cur = m_current;
    if (cur == nullptr)
        THROW_EX() << "Can't find object, PropertiesRegister isn't connected to any object";
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
                return std::make_pair(nullptr, nullptr);
            cur = parent;
            continue;
        }

        {
            auto it = props.m_objects.find(pathElem);
            if (it != props.m_objects.end()) {
                auto* registrable = dynamic_cast<IRegistrable*>(it->second);
                if (!registrable) {
                    if (pathIt + 1 != path.path.end())
                        return std::make_pair(nullptr, nullptr);
                    return std::make_pair(&props, nullptr);
                }
                cur = registrable;
                continue;
            }
        }

        {
            auto it = props.m_properties.find(pathElem);
            if (it != props.m_properties.end()) {
                if (pathIt + 1 != path.path.end())
                    return std::make_pair(nullptr, nullptr);
                return std::make_pair(&props, nullptr);
            }
        }

        return std::make_pair(nullptr, nullptr);
    }

    auto& props = cur->properties();
    if (props.m_parent == nullptr)
        return std::make_pair(nullptr, &props);
    return std::make_pair(&props.m_parent->properties(), &props);
}

std::pair<const PropertiesRegister*, const PropertiesRegister*> PropertiesRegister::find(
    const ObjectTreePath& path) const
{
    return const_cast<PropertiesRegister*>(this)->find(path);
}

void PropertiesRegister::add(
    const std::string& name,
    const std::shared_ptr<IValue>& value)
{
    if (name.empty())
        THROW_EX() << "Can't register anonymous property";
    m_properties[name] = value;
}

void PropertiesRegister::add(
    const std::string& name,
    IObject* obj)
{
    if (name.empty())
        THROW_EX() << "Can't register object without name, anonymous objects should get registrable ID";
    m_objects[name] = obj;
}
}
