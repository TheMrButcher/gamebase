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
    objPosition->setPosition(m_skin->createOffset(m_list.size()));
    m_list.addObject(obj);
}

void LinearLayout::setAssociatedSelectable(ISelectable* selectable)
{
    m_list.setAssociatedSelectable(selectable);
}

IObject* LinearLayout::find(
    const Vec2& point, const Transform2& globalPosition)
{
    if (!isVisible())
        return nullptr;

    auto fullPosition = position() * globalPosition;
    return m_list.find(point, fullPosition);
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
        IDrawable* drawableObj = dynamic_cast<IDrawable*>(it->get());
        if (!drawableObj)
            THROW_EX() << "Can't place object in LinearLayout, it isn't Drawable";
        drawableObj->setBox(box);
        IPositionable* posObj = dynamic_cast<IPositionable*>(it->get());
        if (!posObj)
            THROW_EX() << "Can't place object in LinearLayout, it isn't Positionable";
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
        m_skin->setSize(extent.width(), extent.height());
    else
        m_skin->setSize(0.0f, 0.0f);
    setPositionBoxes(allowedBox, m_skin->box());
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
