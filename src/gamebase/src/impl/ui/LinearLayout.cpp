#include <stdafx.h>
#include <gamebase/impl/ui/LinearLayout.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

LinearLayout::LinearLayout(
    const std::shared_ptr<LinearLayoutSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
    : OffsettedPosition(position)
    , Drawable(this)
    , m_skin(skin)
{
    m_list.setParentPosition(this);
}

int LinearLayout::addObject(const std::shared_ptr<IObject>& obj)
{
    auto* objPosition = dynamic_cast<OffsettedPosition*>(obj.get());
    if (!objPosition)
        THROW_EX() << "Can't add object with not OffsettedPosition";
    objPosition->setRelativeOffset(m_skin->createOffset(m_list.size()));
    m_list.addObject(obj);
    update();
    return static_cast<int>(m_list.size()) - 1;
}

void LinearLayout::insertObject(int id, const std::shared_ptr<IObject>& obj)
{
    size_t index = static_cast<size_t>(id);
    if (index == m_list.size()) {
        addObject(obj);
        return;
    }
    auto* objPosition = dynamic_cast<OffsettedPosition*>(obj.get());
    if (!objPosition)
        THROW_EX() << "Can't insert object with not OffsettedPosition";
    objPosition->setRelativeOffset(m_skin->createOffset(index));
    m_list.replaceObject(id, obj);
    update();
}

void LinearLayout::removeObject(int id)
{
    if (m_list.removeObject(id))
        update();
}

void LinearLayout::removeObject(IObject* obj)
{
    removeObject(m_list.findObject(obj));
}

void LinearLayout::setFixedBox(float width, float height)
{
    m_skin->setFixedBox(width, height);
    update();
}

void LinearLayout::update()
{
    if (!m_allowedBox.isValid())
        return;
    setBox(m_allowedBox);
    loadResources();
}

void LinearLayout::setAssociatedSelectable(ISelectable* selectable)
{
    m_list.setAssociatedSelectable(selectable);
}

std::shared_ptr<IObject> LinearLayout::findChildByPoint(const Vec2& point) const
{
    if (!isVisible())
        return nullptr;
    return m_list.findChildByPoint(position().inversed() * point);
}

void LinearLayout::loadResources()
{
    m_skin->loadResources();
    m_list.loadResources();
}

void LinearLayout::drawAt(const Transform2& position) const
{
    m_skin->draw(position);
    m_list.draw(position);
}

namespace {
BoundingBox placeObjects(ObjectsCollection& collection, const BoundingBox& originBox, bool isHorizontal)
{
    BoundingBox box = originBox;
    BoundingBox extent;
    for (auto it = collection.begin(); it != collection.end(); ++it) {
        OffsettedPosition* posObj = dynamic_cast<OffsettedPosition*>(it->get());
        if (!posObj)
            THROW_EX() << "Can't place object in LinearLayout, it isn't OffsettedPosition";

        auto movedBox = box;
        movedBox.move(posObj->offset<IRelativeOffset>()->count(box, box));
        if (isHorizontal)
            box.topRight.x = originBox.topRight.x - (movedBox.bottomLeft.x - box.bottomLeft.x);
        else
            box.bottomLeft.y = originBox.bottomLeft.y - (movedBox.topRight.y - box.topRight.y);

        IDrawable* drawableObj = dynamic_cast<IDrawable*>(it->get());
        if (!drawableObj)
            THROW_EX() << "Can't place object in LinearLayout, it isn't Drawable";
        drawableObj->setBox(box);
        BoundingBox objBox = drawableObj->box();
        objBox.move(posObj->position().offset);
        if (isHorizontal)
            box.bottomLeft.x = objBox.topRight.x;
        else
            box.topRight.y = objBox.bottomLeft.y;
        extent.add(objBox);
    }
    return extent;
}
}

void LinearLayout::setBox(const BoundingBox& allowedBox)
{
    m_skin->setBox(allowedBox);

    bool isHorizontal = m_skin->direction() == Direction::Horizontal;
    auto extent = placeObjects(m_list, m_skin->listBox(), isHorizontal);
    if (extent.isValid())
        m_skin->setExtent(extent);
    else
        m_skin->setExtent(BoundingBox(Vec2(0, 0), Vec2(0, 0)));
    setPositionBoxes(allowedBox, m_skin->box());
    m_allowedBox = allowedBox;
}

void LinearLayout::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject(m_skin.get());
    builder->registerObject(&m_list);
}

void LinearLayout::serialize(Serializer& s) const
{
    s << "position" << m_offset << "skin" << m_skin << "list" << m_list.objects();
}

std::unique_ptr<IObject> deserializeLinearLayout(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<LinearLayoutSkin>, skin);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, list);
    std::unique_ptr<LinearLayout> result(new LinearLayout(skin, position));
    for (auto it = list.begin(); it != list.end(); ++it)
        result->addObject(*it);
    return std::move(result);
}

REGISTER_CLASS(LinearLayout);

} }
