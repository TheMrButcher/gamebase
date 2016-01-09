#pragma once

#include "TreeViewSkin.h"

namespace gamebase { namespace editor {

class TreeView : public OffsettedPosition, public Drawable, public Registrable, public IFindable {
public:
    static const int ROOT_ID = 0;

    TreeView(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<TreeViewSkin>& skin);

    int addObject(int parentID, const std::shared_ptr<IObject>& obj);
    std::shared_ptr<IObject> getObject(int id) const;
    void removeSubtree(int id);
    void removeChildren(int id);
    void addSubtree(int parentID, const TreeView& tree);
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

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
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

    struct Node {
        Node();
        Node(
            int parentID,
            const std::shared_ptr<IObject>& obj,
            const std::shared_ptr<IRelativeBox>& box);

        int parentID;
        std::shared_ptr<ToggleButton> openButton;
        std::shared_ptr<IRelativeBox> subtreeBox;
        std::shared_ptr<IObject> obj;
        IDrawable* drawObj;
        IPositionable* posObj;
        std::vector<int> children;
        bool isOpened;
    };
    
    void setOpened(int id, bool value);
    void setVisible(int id, bool value);
    void setVisibleChildren(int id, bool value);

    std::shared_ptr<TreeViewSkin> m_skin;
    int m_nextID;
    std::map<int, Node> m_tree;
    std::shared_ptr<ScrollableArea> m_area;
    BoundingBox m_parentBox;
};

} }
