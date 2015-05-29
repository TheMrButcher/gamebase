#pragma once

#include <gamebase/serial/JsonFormat.h>
#include <gamebase/utils/Exception.h>
#include <functional>
#include <unordered_map>

namespace Json {
class Value;
}

namespace gamebase { namespace editor {

class DesignModel {
public:
    typedef std::function<void(Json::Value*)> UpdateModelFunc;

    static const int ROOT_ID = 0;

    DesignModel();
    ~DesignModel();

    struct Node {
        enum Type {
            Object,
            Array
        };

        Node() : parentID(-1), id(-1), dataPtr(nullptr) {}
        Node(int parentID, int nodeID, Json::Value* data, Type type)
            : parentID(parentID), id(nodeID), dataPtr(data), type(type)
        {}

        Json::Value& data() { return *dataPtr; }

        int parentID;
        size_t indexInParent;
        std::string nameInParent; 

        int id;
        Type type;
        std::vector<int> children;
        std::vector<UpdateModelFunc> updaters;
        Json::Value* dataPtr;
    };

    Node& add(int parentID, Node::Type type, std::string name);

    bool has(int nodeID) const
    {
        auto it = m_tree.find(nodeID);
        return it != m_tree.end();
    }

    Node& get(int nodeID)
    {
        auto it = m_tree.find(nodeID);
        if (it == m_tree.end())
            THROW_EX() << "Can't find in DesignModel node with id=" << nodeID;
        return it->second;
    }

    void update() { update(ROOT_ID); }
    std::string toString(JsonFormat::Enum format);

private:
    void update(int nodeID);

    std::unordered_map<int, Node> m_tree;
    std::unique_ptr<Json::Value> m_jsonData;
    int m_nextID;
};

} }
