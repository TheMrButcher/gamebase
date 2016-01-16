#include "TreeView.h"
#include <gamebase/geom/RectGeometry.h>
#include <gamebase/geom/PointGeometry.h>

namespace gamebase { namespace editor {

TreeView::TreeView(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<TreeViewSkin>& skin)
    : OffsettedPosition(position)
    , Drawable(this)
    , m_skin(skin)
    , m_nextID(1)
{
    m_area = skin->createTreeArea();
    m_area->setRecountObjectsBoxes(false);
    m_area->setParentPosition(this);

    m_tree[ROOT_ID] = Node();
    m_tree[ROOT_ID].isOpened = true;
}

int TreeView::addObject(int parentID, const std::shared_ptr<IObject>& obj)
{
    auto* objPosition = dynamic_cast<OffsettedPosition*>(obj.get());
    if (!objPosition)
        THROW_EX() << "Can't add object with not OffsettedPosition";
    objPosition->setRelativeOffset(m_skin->createOffset());
    int newID = m_nextID++;
    auto& node = m_tree[newID];
    node = Node(parentID, obj, m_skin->createSubtreeBox());
    auto& parent = require(parentID);
    if (parentID != ROOT_ID && parent.children.empty()) {
        parent.openButton = m_skin->createOpenButton();
        if (parent.openButton) {
            m_area->objects().addObject(parent.openButton);
            parent.openButton->setVisible(parent.drawObj->isVisible());
            parent.openButton->setCallback(std::bind(&TreeView::setOpened, this, parentID, true));
            parent.openButton->setUnpressCallback(std::bind(&TreeView::setOpened, this, parentID, false));
        }
    }
    if (parentID != ROOT_ID && (parent.drawObj->isVisible() == false || parent.isOpened == false))
        node.drawObj->setVisible(false);
    else
        node.drawObj->setVisible(true);
    parent.children.push_back(newID);
    m_area->objects().addObject(obj);
    return newID;
}

std::shared_ptr<IObject> TreeView::getObject(int id) const
{
    return require(id).obj;
}

void TreeView::removeSubtree(int id)
{
    auto& node = require(id);
    int parentID = node.parentID;
    auto& parent = require(parentID);
    {
        auto it = std::find(parent.children.begin(), parent.children.end(), id);
        if (it == parent.children.end())
            THROW_EX() << "Tree is broken, node #" << id << " is not child of its parent node #" << parentID;
        parent.children.erase(it);
        if (parent.children.empty()) {
            parent.openButton.reset();
            parent.isOpened = false;
        }
    }
    removeNodeAndChildren(id);
    collectObjects();
}

void TreeView::removeChildren(int id)
{
    auto& node = require(id);
    node.isOpened = false;
    removeChildrenImpl(id);
    collectObjects();
}

void TreeView::addSubtree(int parentID, const TreeView& tree)
{
    addNodeChildren(parentID, tree, ROOT_ID);
}

void TreeView::swapInParents(int id1, int id2)
{
    auto parentID1 = require(id1).parentID;
    auto& parent1 = require(parentID1);
    auto it1 = std::find(parent1.children.begin(), parent1.children.end(), id1);
    if (it1 == parent1.children.end())
        THROW_EX() << "Can't swap, tree is broken, node #" << id1 << " is not child of its parent node #" << parentID1;
    
    auto parentID2 = require(id2).parentID;
    auto& parent2 = require(parentID2);
    auto it2 = std::find(parent2.children.begin(), parent2.children.end(), id2);
    if (it2 == parent1.children.end())
        THROW_EX() << "Can't swap, tree is broken, node #" << id2 << " is not child of its parent node #" << parentID2;

    std::swap(*it1, *it2);
}

void TreeView::clear()
{
    m_tree.clear();
    m_nextID = 1;
    m_tree[ROOT_ID] = Node();
    m_tree[ROOT_ID].isOpened = true;
    collectObjects();
}

std::shared_ptr<IObject> TreeView::findChildByPoint(const Vec2& point) const
{
    if (!isVisible())
        return nullptr;
    return m_area->findChildByPoint(position().inversed() * point);
}

void TreeView::loadResources()
{
    m_skin->loadResources();
    m_area->loadResources();
}

void TreeView::drawAt(const Transform2& position) const
{
    m_skin->draw(position);
    m_area->draw(position);
}

void TreeView::setBox(const BoundingBox& allowedBox)
{
    m_skin->setBox(allowedBox);
    countBoxes();
    m_offset->setBoxes(allowedBox, box());
}

void TreeView::removeChildrenImpl(int id)
{
    auto& node = require(id);
    for (auto it = node.children.begin(); it != node.children.end(); ++it)
        removeNodeAndChildren(*it);
    node.children.clear();
    node.openButton.reset();
}

void TreeView::removeNodeAndChildren(int id)
{
    removeChildrenImpl(id);
    m_tree.erase(id);
}

void TreeView::collectObjects()
{
    m_area->objects().clear();
    collectObjects(ROOT_ID);
}

void TreeView::collectObjects(int id)
{
    auto& node = require(id);
    if (node.openButton)
        m_area->objects().addObject(node.openButton);
    if (node.obj)
        m_area->objects().addObject(node.obj);
    for (auto it = node.children.begin(); it != node.children.end(); ++it)
        collectObjects(*it);
}

void TreeView::addNodeChildren(int parentID, const TreeView& tree, int nodeID)
{
    const auto& node = tree.require(nodeID);
    for (auto it = node.children.begin(); it != node.children.end(); ++it)
        addSubtree(parentID, tree, *it);
}

void TreeView::addSubtree(int parentID, const TreeView& tree, int nodeID)
{
    const auto& node = tree.require(nodeID);
    int id = addObject(parentID, node.obj);
    addNodeChildren(id, tree, nodeID);
}

float TreeView::setChildrenBox(const BoundingBox& parentBox, int id)
{
    auto& node = require(id);
    auto curBox = parentBox;
    float bottom = curBox.topRight.y;
    if (node.isOpened) {
        for (auto it = node.children.begin(); it != node.children.end(); ++it) {
            bottom = setSubtreeBox(curBox, *it);
            curBox.topRight.y = bottom;
        }
    }
    return bottom;
}

float TreeView::setSubtreeBox(const BoundingBox& parentBox, int id)
{
    auto& node = require(id);
    node.subtreeBox->setParentBox(parentBox);
    auto subtreeBox = node.subtreeBox->get();
    node.drawObj->setBox(subtreeBox);
    auto objBox = node.drawObj->box();
    auto objPos = node.posObj->position().offset;
    objBox.move(objPos);
    if (node.openButton) {
        auto openButtonBox = subtreeBox;
        openButtonBox.bottomLeft.y = objBox.bottomLeft.y;
        openButtonBox.topRight.x = objBox.bottomLeft.x;
        node.openButton->setBox(openButtonBox);
    }
    subtreeBox.topRight.y = objBox.bottomLeft.y;
    return setChildrenBox(subtreeBox, id);
}

void TreeView::countBoxes()
{
    auto treeBox = m_skin->treeBox();
    treeBox.topRight.x = std::numeric_limits<float>::max();
    treeBox.bottomLeft.y = std::numeric_limits<float>::lowest();
    setChildrenBox(treeBox, ROOT_ID);
    m_area->setBox(m_skin->box());
}

TreeView::Node::Node()
    : parentID(-1)
    , drawObj(nullptr)
    , posObj(nullptr)
    , isOpened(false)
{}

TreeView::Node::Node(
    int parentID,
    const std::shared_ptr<IObject>& obj,
    const std::shared_ptr<IRelativeBox>& box)
    : parentID(parentID)
    , subtreeBox(box)
    , obj(obj)
    , isOpened(false)
{
    if (!(drawObj = dynamic_cast<IDrawable*>(obj.get())))
        THROW_EX() << "Can't create node, object is not Drawable";
    if (!(posObj = dynamic_cast<IPositionable*>(obj.get())))
        THROW_EX() << "Can't create node, object is not Positionable";
}
    
void TreeView::setOpened(int id, bool value)
{
    auto& node = require(id);
    node.isOpened = value;
    setVisibleChildren(id, value);
    countBoxes();
    loadResources();
}

void TreeView::setVisible(int id, bool value)
{
    auto& node = require(id);
    if (node.openButton)
        node.openButton->setVisible(value);
    node.drawObj->setVisible(value);
    if (node.isOpened)
        setVisibleChildren(id, value);
}
    
void TreeView::setVisibleChildren(int id, bool value)
{
    auto& node = require(id);
    for (auto it = node.children.begin(); it != node.children.end(); ++it)
        setVisible(*it, value);
}

const TreeView::Node& TreeView::require(int id) const
{
    return const_cast<TreeView*>(this)->require(id);
}

TreeView::Node& TreeView::require(int id)
{
    auto it = m_tree.find(id);
    if (it == m_tree.end())
        THROW_EX() << "Can't get object, no node with id: " << id;
    return it->second;
}

} }
