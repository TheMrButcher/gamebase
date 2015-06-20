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
        std::string nameInParent; 

        int id;
        Type type;
        Json::Value* dataPtr;

        const std::vector<UpdateModelFunc>& updaters() const { return m_updaters; }
        const std::vector<int>& children() const { return m_children; }

    private:
        friend class DesignModel;

        void remove(int id);

        std::vector<UpdateModelFunc> m_updaters;
        std::vector<int> m_children;
        std::unordered_map<int, size_t> m_positions;
    };

    Node& add(int parentID, Node::Type type, std::string name);

    Node& addFictiveNode();

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
    void update(int nodeID);
    std::string toString(JsonFormat::Enum format);

    int nextID() const { return m_nextID; }

    int addUpdater(int nodeID, const UpdateModelFunc& updater);
    void remove(int id);
    void clearNode(int id);

private:
    void removeInternal(int id);
    void removeContent(int id);

    std::unordered_map<int, Node> m_tree;
    std::unordered_map<int, int> m_updaterHolders;
    std::unique_ptr<Json::Value> m_jsonData;
    std::unordered_map<int, std::unique_ptr<Json::Value>> m_fictiveData;

    int m_nextID;
};

} }
