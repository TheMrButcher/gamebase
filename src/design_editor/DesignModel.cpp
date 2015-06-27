#include "DesignModel.h"
#include <json/value.h>
#include <json/writer.h>

namespace gamebase { namespace editor {
namespace {
const std::string ROOT_CHILD = "OBJ";
}
   
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
    parent.m_positions[nodeID] = parent.m_children.size();
    parent.m_children.push_back(nodeID);
    return result;
}
    
DesignModel::Node& DesignModel::addFictiveNode()
{
    int id = m_nextID++;
    Node node;
    node.id = id;
    node.type = Node::Object;
    auto& result = m_tree[id];
    result = std::move(node);
    return result;
}

std::string DesignModel::toString(JsonFormat::Enum format)
{
    auto jsonValue = toJsonValue(ROOT_ID);
    if (jsonValue->size() != 1)
        THROW_EX() << "Root object is in broken state";
    if (!jsonValue->isMember(ROOT_CHILD))
        THROW_EX() << "Root object is in broken state, can't find member 'OBJ'";
    if (format == JsonFormat::Fast) {
        Json::FastWriter writer;
        return writer.write((*jsonValue)[ROOT_CHILD]);
    }

    Json::StyledWriter writer;
    return writer.write((*jsonValue)[ROOT_CHILD]);
}

std::unique_ptr<Json::Value> DesignModel::toJsonValue(int nodeID)
{
    std::unique_ptr<Json::Value> jsonValue(new Json::Value(Json::objectValue));
    fillJsonValue(nodeID, *jsonValue);
    return std::move(jsonValue);
}

void DesignModel::fillJsonValue(int nodeID, Json::Value& dstValue)
{
    auto& node = get(nodeID);
    for (auto it = node.m_updaters.begin(); it != node.m_updaters.end(); ++it)
        (*it)(&dstValue);
    for (auto it = node.m_children.begin(); it != node.m_children.end(); ++it) {
        auto& childNode = get(*it);
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
    auto& node = get(nodeID);
    if (node.type == Node::Array)
        node.m_positions[updaterID] = node.m_updaters.size();
    node.m_updaters.push_back(updater);
    m_updaterHolders[updaterID] = nodeID;
    return updaterID;
}


void DesignModel::Node::remove(int id)
{
    if (type != Array)
        THROW_EX() << "Can't remove entity from object";
    auto itRemoved = m_positions.find(id);
    if (itRemoved == m_positions.end())
        THROW_EX() << "Can't find position of entity with id=" << id;
    size_t removedPosition = itRemoved->second;
    if (m_updaters.size() > removedPosition)
        m_updaters.erase(m_updaters.begin() + removedPosition);
    else if (m_children.size() > removedPosition)
        m_children.erase(m_children.begin() + removedPosition);
    else
        THROW_EX() << "Position of entity with id=" << id <<
            " is not found in nor updaters neither children";
    m_positions.erase(itRemoved);
    for (auto it = m_positions.begin(); it != m_positions.end(); ++it)
        if (it->second > removedPosition)
            it->second = it->second - 1;
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
    node.m_updaters.clear();
    node.m_positions.clear();
}

void DesignModel::removeInternal(int id)
{
    removeContent(id);
    m_tree.erase(id);
}

void DesignModel::removeContent(int id)
{
    auto& node = get(id);
    for (auto it = node.m_children.begin(); it != node.m_children.end(); ++it)
        removeInternal(*it);
    if (node.type == Node::Array) {
        for (auto itPos = node.m_positions.begin(); itPos != node.m_positions.end(); ++itPos)
            m_updaterHolders.erase(itPos->first);
    }
}

} }
