#include <stdafx.h>
#include <gamebase/engine/LinearLayout.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

LinearLayout::LinearLayout(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<LinearLayoutSkin>& skin)
    : OffsettedPosition(position)
    , Drawable(this)
    , m_skin(skin)
{
    m_list.setParentPosition(this);
}

void LinearLayout::addObject(const std::shared_ptr<IObject>& obj)
{
    auto* objPosition = dynamic_cast<OffsettedPosition*>(obj.get());
    if (!objPosition)
        THROW_EX() << "Can't add object with not OffsettedPosition";
    objPosition->setRelativeOffset(m_skin->createOffset(m_list.size()));
    m_list.addObject(obj);
    if (m_allowedBox.isValid()) {
        setBox(m_allowedBox);
        loadResources();
    }
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
            movedBox.topRight.x = box.topRight.x;
        else
            movedBox.bottomLeft.y = box.bottomLeft.y;
        box = movedBox;

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
        extent.enlarge(objBox);
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
    builder->registerObject("skin", m_skin.get());
    builder->registerObject("objects", &m_list);
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
    std::unique_ptr<LinearLayout> result(new LinearLayout(position, skin));
    for (auto it = list.begin(); it != list.end(); ++it)
        result->addObject(*it);
    return std::move(result);
}

REGISTER_CLASS(LinearLayout);

}
