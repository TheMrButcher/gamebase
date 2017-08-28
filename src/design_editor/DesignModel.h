/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/serial/JsonFormat.h>
#include <gamebase/tools/Exception.h>
#include <functional>
#include <unordered_map>
#include <memory>

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

        Node() : parentID(-1), id(-1) {}
        Node(int parentID, int nodeID, Type type)
            : parentID(parentID), id(nodeID), type(type)
        {}

        int parentID;
        std::string nameInParent; 

        int id;
        Type type;

        void remove(int id);

        struct Element {
            Element() {}
            Element(int id) : type(ChildNode), id(id) {}
            Element(int id, const UpdateModelFunc& updater) : type(Updater), id(id), updater(updater) {}

            enum Type {
                Updater,
                ChildNode
            } type;
            int id;
            UpdateModelFunc updater;
        };

        const std::vector<Element>& children() const { return m_children; }
        size_t updatersNum() const;
        int position(int id) const;
        void swap(int id1, int id2);

    private:
        friend class DesignModel;

        void addChild(int id);
        void addUpdater(int id, const UpdateModelFunc& updater);

        std::vector<Element> m_children;
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
    
    std::string toString(impl::JsonFormat::Enum format);
    std::string toString(int nodeID, impl::JsonFormat::Enum format);
    std::unique_ptr<Json::Value> toJsonValue(int nodeID);

    int nextID() const { return m_nextID; }

    int addUpdater(int nodeID, const UpdateModelFunc& updater);
    void remove(int id);
    void clearNode(int id);
    void clear();

private:
    void removeInternal(int id);
    void removeContent(int id);
    void fillJsonValue(int nodeID, Json::Value& dstValue);

    std::unordered_map<int, Node> m_tree;
    std::unordered_map<int, int> m_updaterHolders;

    int m_nextID;
};

} }
