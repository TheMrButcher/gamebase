/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/reg/PropertiesRegister.h>
#include <gamebase/impl/reg/IRegistrable.h>
#include <vector>
#include <sstream>
#include <deque>
#include <iostream>

namespace gamebase { namespace impl {

namespace {

template <typename Collection>
typename Collection::const_iterator findVal(const Collection& c, const std::string& name)
{
    for (auto it = c.begin(); it != c.end(); ++it)
        if (it->name == name)
            return it;
    return c.end();
}

template <typename Collection>
typename Collection::iterator findVal(Collection& c, const std::string& name)
{
    for (auto it = c.begin(); it != c.end(); ++it)
        if (it->name == name)
            return it;
    return c.end();
}

}

PropertiesRegister::PropertiesRegister()
    : m_current(nullptr)
    , m_parent(nullptr)
{}

struct ObjectTreePath {
    ObjectTreePath(const std::string& pathStr)
        : pathStr(pathStr)
    {
        if (pathStr.empty())
            THROW_EX() << "Can't build ObjectTreePath, empty string";
        isAbsolute = pathStr[0] == '/';
        isInSubtree = pathStr[0] != '.';
        size_t index = 0;
        size_t next;
        while ((next = pathStr.find_first_of("/.#", index)) != pathStr.npos) {
            if (next != index)
                path.push_back(pathStr.substr(index, next - index));
            index = next + 1;
        }
        if (index < pathStr.length())
            path.push_back(pathStr.substr(index));
    }

    bool isAbsolute;
    bool isInSubtree;
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
        if (findVal(props->m_properties, path.path.back()) != props->m_properties.end())
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
        if (findVal(props->m_objects, path.path.back()) != props->m_objects.end())
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
        auto it = findVal(props->m_properties, path.path.back());
        if (it == props->m_properties.end())
            THROW_EX() << "Can't find property, name: " << name;
        return it->prop;
    }
    THROW_EX() << "Can't find object that holds property, name: " << name;
}

IObject* PropertiesRegister::tryGetAbstractObject(const std::string& name) const
{
    ObjectTreePath path(name);
    auto parentAndNode = find(path);
    if (parentAndNode.second)
        return parentAndNode.second->m_current;
    if (auto* props = parentAndNode.first) {
        auto it = findVal(props->m_objects, path.path.back());
        if (it != props->m_objects.end())
            return it->obj;
    }
    return nullptr;
}

IObject* PropertiesRegister::getAbstractObject(const std::string& name) const
{
    auto result = tryGetAbstractObject(name);
    if (!result)
        THROW_EX() << "Can't find object, name: " << name;
    return result;
}

std::pair<PropertiesRegister*, PropertiesRegister*> PropertiesRegister::find(
    const ObjectTreePath& path)
{
    static const std::pair<PropertiesRegister*, PropertiesRegister*> NOT_FOUND =
        std::make_pair(nullptr, nullptr);

    if (!path.isAbsolute && path.path.empty())
        return NOT_FOUND;

    if (path.isInSubtree) {
        if (path.isAbsolute)
            THROW_EX() << "Unexpected error: search path is in suntree and absolute at the same time";
        auto searchPath = path;
        searchPath.isInSubtree = false;
        std::deque<PropertiesRegister*> queue;
        queue.push_back(this);
        while (!queue.empty()) {
            auto* cur = queue.front();
            queue.pop_front();
            auto tmpResult = cur->find(searchPath);
            if (tmpResult != NOT_FOUND)
                return tmpResult;
            for (auto it = cur->m_objects.begin(); it != cur->m_objects.end(); ++it) {
                if (auto* registrable = dynamic_cast<IRegistrable*>(it->obj)) {
                    queue.push_back(&registrable->properties());
                }
            }
            for (auto it = cur->m_anonObjects.begin(); it != cur->m_anonObjects.end(); ++it) {
                if (auto* registrable = dynamic_cast<IRegistrable*>(*it)) {
                    queue.push_back(&registrable->properties());
                }
            }
        }
        return NOT_FOUND;
    }

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
        {
            auto it = findVal(props.m_objects, pathElem);
            if (it != props.m_objects.end()) {
                auto* registrable = dynamic_cast<IRegistrable*>(it->obj);
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
            auto it = findVal(props.m_properties, pathElem);
            if (it != props.m_properties.end()) {
                if (pathIt + 1 != path.path.end())
                    return NOT_FOUND;
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

void PropertiesRegister::add(const std::string& name, const std::shared_ptr<IValue>& value)
{
    if (name.empty())
        THROW_EX() << "Can't register anonymous property";
    if (m_properties.empty())
        m_properties.reserve(10);
    m_properties.push_back(NamedProperty(name, value));
}

void PropertiesRegister::add(IObject* obj)
{
    m_anonObjects.insert(obj);
}

void PropertiesRegister::add(const std::string& name, IObject* obj)
{
    if (name.empty()) {
        add(obj);
        return;
    }
    if (m_objects.empty())
        m_objects.reserve(4);
    m_objects.push_back(NamedObject(name, obj));
}

void PropertiesRegister::remove(const std::string& name)
{
    {
        auto it = findVal(m_objects, name);
        if (it != m_objects.end()) {
            m_objects.erase(it);
            return;
        }
    }
    {
        auto it = findVal(m_properties, name);
        if (it != m_properties.end()) {
            m_properties.erase(it);
            return;
        }
    }
}

void PropertiesRegister::remove(IObject* obj)
{
    if (auto* registrable = dynamic_cast<IRegistrable*>(obj)) {
        auto name = registrable->properties().name();
        if (!name.empty())
            remove(name);
    }
    m_anonObjects.erase(obj);
}

bool PropertiesRegister::empty() const
{
    return m_properties.empty() && m_objects.empty() && m_anonObjects.empty();
}

void PropertiesRegister::clear()
{
    m_properties.clear();
    m_objects.clear();
    m_anonObjects.clear();
}

} }
