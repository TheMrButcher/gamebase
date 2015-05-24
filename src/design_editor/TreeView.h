#pragma once

#include "TreeViewSkin.h"
#include <gamebase/engine/ObjectsCollection.h>

namespace gamebase { namespace editor {

class TreeView : public OffsettedPosition, public Drawable, public Registrable, public IFindable {
public:
    static const int ROOT_ID = 0;

    TreeView(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<TreeViewSkin>& skin);

    int addObject(int parentID, const std::shared_ptr<IObject>& obj);
    void removeSubtree(int id);
    void removeChildren(int id);
    void addSubtree(int parentID, const TreeView& tree);

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;

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
        builder->registerObject("objects", &m_objects);
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

    void countBoxes();

    struct Node {
        Node();
        Node(
            int parentID,
            const std::shared_ptr<IObject>& obj,
            const std::shared_ptr<IRelativeBox>& box);

        int parentID;
        std::shared_ptr<PressableButton> openButton;
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
    ObjectsCollection m_objects;
    BoundingBox m_parentBox;
};

} }
