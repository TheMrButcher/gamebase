#include "DesignModel.h"
#include <json/value.h>
#include <json/writer.h>

namespace gamebase { namespace editor {
namespace {
const std::string ROOT_CHILD = "OBJ";
}
   
DesignModel::DesignModel()
    : m_jsonData(new Json::Value(Json::objectValue))
    , m_nextID(1)
{
    m_tree[ROOT_ID] = Node(-1, ROOT_ID, m_jsonData.get(), Node::Object);
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

    size_t indexInParent = 0;
    Json::Value* newValuePtr = nullptr;
    if (parent.type == Node::Object) {
        parent.data()[name] = newValue;
        newValuePtr = &parent.data()[name];
    } else {
        indexInParent = parent.data().size();
        parent.data()[indexInParent] = newValue;
        newValuePtr = &parent.data()[indexInParent];
    }

    int nodeID = m_nextID++;
    Node node(parentID, nodeID, newValuePtr, type);
    node.indexInParent = indexInParent;
    node.nameInParent = std::move(name);
    auto& result = m_tree[nodeID];
    result = std::move(node);
    parent.children.push_back(nodeID);
    return result;
}
    
DesignModel::Node& DesignModel::addFictiveNode()
{
    int id = m_nextID++;
    auto& data = m_fictiveData[id];
    data.reset(new Json::Value(Json::objectValue));
    Node node;
    node.id = id;
    node.type = Node::Object;
    node.dataPtr = data.get();
    auto& result = m_tree[id];
    result = std::move(node);
    return result;
}

std::string DesignModel::toString(JsonFormat::Enum format)
{
    if (m_jsonData->size() != 1)
        THROW_EX() << "Root object is in broken state";
    if (!m_jsonData->isMember(ROOT_CHILD))
        THROW_EX() << "Root object is in broken state, can't find member 'OBJ'";
    if (format == JsonFormat::Fast) {
        Json::FastWriter writer;
        return writer.write((*m_jsonData)[ROOT_CHILD]);
    }

    Json::StyledWriter writer;
    return writer.write((*m_jsonData)[ROOT_CHILD]);
}

void DesignModel::update(int nodeID)
{
    auto& node = get(nodeID);
    if (node.type == Node::Array && !node.updaters.empty())
        node.data().clear();
    for (auto it = node.updaters.begin(); it != node.updaters.end(); ++it)
        (*it)(node.dataPtr);
    for (auto it = node.children.begin(); it != node.children.end(); ++it)
        update(*it);
}

} }
