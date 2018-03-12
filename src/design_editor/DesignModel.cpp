/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "DesignModel.h"
#include <gamebase/impl/serial/constants.h>
#include <json/value.h>
#include <json/writer.h>
#include <algorithm>

namespace gamebase { namespace editor {
namespace {
const std::string ROOT_CHILD = "OBJ";
}

///////////////////////// DESIGN MODEL NODE ///////////////////////////////////

namespace {
struct ElementFinder {
    ElementFinder(int id) : id(id) {}

    bool operator()(const DesignModel::Node::Element& elem) const
    {
        return id == elem.id;
    }

    int id;
};
}

void DesignModel::Node::addChild(int id)
{
    if (type == Array && !m_children.empty() && m_children.back().type != Element::ChildNode)
        THROW_EX() << "Array can't contain elements of different types";
    m_children.push_back(Element(id));
}

void DesignModel::Node::addUpdater(int id, const UpdateModelFunc& updater)
{
    if (type == Array && !m_children.empty() && m_children.back().type != Element::Updater)
        THROW_EX() << "Array can't contain elements of different types";
    m_children.push_back(Element(id, updater));
}

void DesignModel::Node::remove(int id)
{
    auto it = std::find_if(m_children.begin(), m_children.end(), ElementFinder(id));
    if (it != m_children.end())
        m_children.erase(it);
}

size_t DesignModel::Node::updatersNum() const
{
    size_t result = 0;
    for (auto it = m_children.begin(); it != m_children.end(); ++it)
        if (it->type == Element::Updater)
            result++;
    return result;
}

int DesignModel::Node::position(int id) const
{
    auto it = std::find_if(m_children.begin(), m_children.end(), ElementFinder(id));
    if (it == m_children.end())
        return -1;
    return static_cast<int>(it - m_children.begin());
}

void DesignModel::Node::swap(int id1, int id2)
{
    auto it1 = std::find_if(m_children.begin(), m_children.end(), ElementFinder(id1));
    if (it1 == m_children.end())
        return;
    auto it2 = std::find_if(m_children.begin(), m_children.end(), ElementFinder(id2));
    if (it2 == m_children.end())
        return;
    std::swap(*it1, *it2);
}

   
///////////////////////// DESIGN MODEL ////////////////////////////////////////
DesignModel::DesignModel()
    : m_nextID(1)
{
    m_tree[ROOT_ID] = Node(-1, ROOT_ID, Node::Object);
}

DesignModel::~DesignModel() {}

DesignModel::Node& DesignModel::add(
    int parentID, Node::Type type, std::string name)
{
    auto& parent = get(parentID);
    if (parentID == ROOT_ID)
        name = ROOT_CHILD;
    Json::Value newValue;
    if (type == Node::Object)
        newValue = Json::objectValue;
    else
        newValue = Json::arrayValue;

    int nodeID = m_nextID++;
    Node node(parentID, nodeID, type);
    node.nameInParent = std::move(name);
    auto& result = m_tree[nodeID];
    result = std::move(node);
    parent.addChild(nodeID);
    return result;
}

std::string DesignModel::toString(impl::JsonFormat::Enum format)
{
    auto jsonValue = toJsonValue(ROOT_ID);
    if (jsonValue->size() != 1)
        THROW_EX() << "Root object is in broken state";
    if (!jsonValue->isMember(ROOT_CHILD))
        THROW_EX() << "Root object is in broken state, can't find member 'OBJ'";
    auto& root = (*jsonValue)[ROOT_CHILD];
    root[impl::VERSION_TAG] = impl::toString(impl::SerializationVersion::VER3);
    if (format == impl::JsonFormat::Fast) {
        Json::FastWriter writer;
        return writer.write(root);
    }

    Json::StyledWriter writer;
    return writer.write(root);
}

std::string DesignModel::toString(int nodeID, impl::JsonFormat::Enum format)
{
    auto jsonValue = toJsonValue(nodeID);
    (*jsonValue)[impl::VERSION_TAG] = impl::toString(impl::SerializationVersion::VER3);
    if (format == impl::JsonFormat::Fast) {
        Json::FastWriter writer;
        return writer.write(*jsonValue);
    }
    Json::StyledWriter writer;
    return writer.write(*jsonValue);
}

std::unique_ptr<Json::Value> DesignModel::toJsonValue(int nodeID)
{
    if (m_flush)
        m_flush();
    std::unique_ptr<Json::Value> jsonValue(new Json::Value(Json::objectValue));
    fillJsonValue(nodeID, *jsonValue);
    return std::move(jsonValue);
}

void DesignModel::fillJsonValue(int nodeID, Json::Value& dstValue)
{
    auto& node = get(nodeID);
    for (auto it = node.m_children.begin(); it != node.m_children.end(); ++it) {
        if (it->type == Node::Element::Updater) {
            it->updater(&dstValue);
            continue;
        }
        auto& childNode = get(it->id);
        Json::Value childJsonValue;
        if (childNode.type == Node::Object)
            childJsonValue = Json::objectValue;
        else
            childJsonValue = Json::arrayValue;

        Json::Value* childJsonValuePtr = nullptr;
        if (node.type == Node::Object) {
            dstValue[childNode.nameInParent] = childJsonValue;
            childJsonValuePtr = &dstValue[childNode.nameInParent];
        } else {
            childJsonValuePtr = &dstValue.append(childJsonValue);
        }
        fillJsonValue(childNode.id, *childJsonValuePtr);
    }
}

int DesignModel::addUpdater(int nodeID, const DesignModel::UpdateModelFunc& updater)
{
    int updaterID = m_nextID++;
    get(nodeID).addUpdater(updaterID, updater);
    m_updaterHolders[updaterID] = nodeID;
    return updaterID;
}

void DesignModel::remove(int id)
{
    if (id == ROOT_ID)
        THROW_EX() << "Can't remove root node";
    auto nodeIt = m_tree.find(id);
    if (nodeIt != m_tree.end()) {
        auto& node = nodeIt->second;
        int parentID = node.parentID;
        auto& parent = get(parentID);
        parent.remove(id);
        removeInternal(id);
        return;
    }

    auto updaterHolderIt = m_updaterHolders.find(id);
    if (updaterHolderIt != m_updaterHolders.end()) {
        auto nodeID = updaterHolderIt->second;
        auto& node = get(nodeID);
        node.remove(id);
        m_updaterHolders.erase(id);
        return;
    }

    THROW_EX() << "Entity with id=" << id << " is nor node neither updater";
}

void DesignModel::clearNode(int id)
{
    auto& node = get(id);
    removeContent(id);
    node.m_children.clear();
}

void DesignModel::clear()
{
    m_tree.clear();
    m_updaterHolders.clear();
    m_nextID = 1;
    m_tree[ROOT_ID] = Node(-1, ROOT_ID, Node::Object);
}

void DesignModel::removeInternal(int id)
{
    removeContent(id);
    m_tree.erase(id);
}

void DesignModel::removeContent(int id)
{
    auto& node = get(id);
    for (auto it = node.m_children.begin(); it != node.m_children.end(); ++it) {
        if (it->type == Node::Element::Updater) {
            m_updaterHolders.erase(it->id);
        } else {
            removeInternal(it->id);
        }
    }
}

} }
