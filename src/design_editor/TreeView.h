/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "TreeViewSkin.h"
#include <unordered_map>

namespace gamebase { namespace editor {

class TreeView : public impl::OffsettedPosition, public impl::Drawable, public impl::Registrable, public impl::IFindable {
public:
    static const int ROOT_ID = 0;

    TreeView(
        const std::shared_ptr<impl::IRelativeOffset>& position,
        const std::shared_ptr<TreeViewSkin>& skin);

    int addObject(int parentID, const std::shared_ptr<impl::IObject>& obj);
    std::shared_ptr<IObject> getObject(int id) const;
    void removeSubtree(int id);
    void removeChildren(int id);
    void addSubtree(int parentID, const TreeView& tree);
    void swapInParents(int id1, int id2);
    const std::vector<int>& children(int id) const { return require(id).children; } 
    int parentID(int id) const { return require(id).parentID; } 
    void clear();

    int nextID() const { return m_nextID; }

    void countBoxes();

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual void registerObject(impl::PropertiesRegisterBuilder* builder) override
    {
        builder->registerObject("skin", m_skin.get());
        builder->registerObject("area", m_area.get());
    }

private:
    void removeChildrenImpl(int id);

    void removeNodeAndChildren(int id);

    void collectObjects();

    void collectObjects(int id);

    void addNodeChildren(int parentID, const TreeView& tree, int nodeID);

    void addSubtree(int parentID, const TreeView& tree, int nodeID);

    float setChildrenBox(const BoundingBox& parentBox, int id);

    float setSubtreeBox(const BoundingBox& parentBox, int id);
    
    void setOpened(int id, bool value);
    void setOpenedCallback(int id, impl::ToggleButton* button);
    void setVisible(int id, bool value);
    void setVisibleChildren(int id, bool value);

    struct Node {
        Node();
        Node(
            int parentID,
            const std::shared_ptr<impl::IObject>& obj,
            const std::shared_ptr<impl::IRelativeBox>& box);

        int parentID;
        std::shared_ptr<impl::ToggleButton> openButton;
        std::shared_ptr<impl::IRelativeBox> subtreeBox;
        std::shared_ptr<impl::IObject> obj;
        impl::IDrawable* drawObj;
        impl::IPositionable* posObj;
        std::vector<int> children;
        bool isOpened;
    };

    const Node& require(int id) const;
    Node& require(int id);

    std::shared_ptr<TreeViewSkin> m_skin;
    int m_nextID;
    std::unordered_map<int, Node> m_tree;
    std::unordered_set<int> m_onCanvas;
    std::shared_ptr<impl::ScrollableArea> m_area;
    BoundingBox m_parentBox;
};

} }
