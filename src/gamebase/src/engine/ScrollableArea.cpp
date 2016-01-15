#include <stdafx.h>
#include <gamebase/engine/ScrollableArea.h>
#include <gamebase/engine/ValueLink.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/geom/PointGeometry.h>
#include <gamebase/geom/RectGeometry.h>
#include <gamebase/graphics/Clipping.h>

namespace gamebase {

class ScrollableArea::ScrollPosition : public IPositionable {
public:
    ScrollPosition(): m_inited(false) {}

    bool isInited() const { return m_inited; }

    void init(const Vec2& base, const Vec2& initialOffset)
    {
        m_baseOffset = base;
        m_offset = initialOffset;
        m_inited = true;
    }

    Vec2 baseOffset() const { return m_baseOffset; }
    Vec2 offset() const { return m_offset; }

    std::shared_ptr<FloatValue> getX() { return std::make_shared<ValueLink<float>>(&m_offset.x); }
    std::shared_ptr<FloatValue> getY() { return std::make_shared<ValueLink<float>>(&m_offset.y); }

    virtual Transform2 position() const override { return ShiftTransform2(m_baseOffset - m_offset); }

private:
    Vec2 m_baseOffset;
    Vec2 m_offset;
    bool m_inited;
};

ScrollableArea::ScrollableArea(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<ScrollableAreaSkin>& skin)
    : OffsettedPosition(position)
    , Drawable(this)
    , m_skin(skin)
    , m_scrollOffset(std::make_shared<ScrollPosition>())
    , m_objects(m_scrollOffset.get())
    , m_recountObjectsBoxes(true)
{
    m_objects.setParentPosition(this);
    m_sysObjects.setParentPosition(this);
    if (m_horScroll = m_skin->createScrollBar(m_scrollOffset->getX(), Direction::Horizontal))
        m_sysObjects.addObject(m_horScroll);
    if (m_vertScroll = m_skin->createScrollBar(m_scrollOffset->getY(), Direction::Vertical))
        m_sysObjects.addObject(m_vertScroll);
}

void ScrollableArea::setAssociatedSelectable(ISelectable* selectable)
{
    m_objects.setAssociatedSelectable(selectable);
    m_sysObjects.setAssociatedSelectable(selectable);
}

void ScrollableArea::update()
{
    setBox(m_parentBox);
    if (m_recountObjectsBoxes) {
        loadResources();
    } else {
        m_skin->loadResources();
        m_sysObjects.loadResources();
    }
}

std::shared_ptr<IObject> ScrollableArea::findChildByPoint(const Vec2& point) const
{
    if (!isVisible())
        return nullptr;

    auto transformedPoint = position().inversed() * point;
    if (auto result = m_sysObjects.findChildByPoint(transformedPoint))
        return result;

    PointGeometry pointGeom(point);
    RectGeometry rectGeom(m_skin->areaBox());
    if (!rectGeom.intersects(&pointGeom, position(), Transform2()))
        return nullptr;
    return m_objects.findChildByPoint(transformedPoint);
}

void ScrollableArea::loadResources()
{
    m_skin->loadResources();
    m_objects.loadResources();
    m_sysObjects.loadResources();
}

void ScrollableArea::drawAt(const Transform2& position) const
{
    m_skin->draw(position);

    setClipBox(position, m_skin->areaBox());
    m_objects.draw(position);
    disableClipping();

    m_sysObjects.draw(position);
}

namespace {
BoundingBox extension(const std::vector<std::shared_ptr<IObject>>& objects)
{
    BoundingBox extent;
    for (auto it = objects.begin(); it != objects.end(); ++it) {
        IDrawable* drawableObj = dynamic_cast<IDrawable*>(it->get());
        if (!drawableObj)
            continue;
        if (!drawableObj->isVisible())
            continue;
        IPositionable* posObj = dynamic_cast<IPositionable*>(it->get());
        if (!posObj)
            continue;
        BoundingBox objBox = drawableObj->box();
        objBox.move(posObj->position().offset);
        extent.enlarge(objBox);
    }
    return extent;
}
}

void ScrollableArea::setBox(const BoundingBox& allowedBox)
{
    m_parentBox = allowedBox;
    m_skin->setBox(allowedBox);
    if (m_recountObjectsBoxes)
        m_objects.setBox(m_skin->areaBox());
    auto extBox = extension(m_objects.objects());
    if (extBox.isValid()) {
        extBox.enlarge(Vec2(m_skin->areaBox().bottomLeft.x, m_skin->areaBox().topRight.y));
        m_skin->setSize(extBox.width(), extBox.height());
        if (m_recountObjectsBoxes) {
            m_objects.setBox(m_skin->areaBox());
            extBox = extension(m_objects.objects());
            m_skin->setSize(extBox.width(), extBox.height());
        }

        Vec2 baseOffset(
            m_skin->areaBox().bottomLeft.x - extBox.bottomLeft.x,
            m_skin->areaBox().topRight.y - extBox.topRight.y);
        baseOffset.y += std::max(0.0f, extBox.height() - m_skin->areaBox().height());
        Vec2 initialOffset = baseOffset;
        if (m_scrollOffset->isInited()) {
            initialOffset = m_scrollOffset->offset();
            initialOffset += baseOffset - m_scrollOffset->baseOffset();
        }
        m_scrollOffset->init(baseOffset, initialOffset);
        if (m_horScroll)
            m_horScroll->setParams(0.0f, extBox.width(), m_skin->areaBox().width());
        if (m_vertScroll)
            m_vertScroll->setParams(0.0f, extBox.height(), m_skin->areaBox().height());
    }
    m_sysObjects.setBox(m_skin->areaBox());
    setPositionBoxes(allowedBox, m_skin->box());
}

void ScrollableArea::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("skin", m_skin.get());
    if (m_horScroll)
        builder->registerObject("horScrollBar", m_horScroll.get());
    if (m_vertScroll)
        builder->registerObject("vertScrollBar", m_vertScroll.get());
    builder->registerObject("objects", &m_objects);
}

void ScrollableArea::serialize(Serializer& s) const
{
    s << "position" << m_offset << "skin" << m_skin << "objects" << m_objects.objects()
        << "recountObjectsBoxes" << m_recountObjectsBoxes;
}

std::unique_ptr<IObject> deserializeScrollableArea(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<ScrollableAreaSkin>, skin);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, objects);
    DESERIALIZE(bool, recountObjectsBoxes);

    std::unique_ptr<ScrollableArea> result(new ScrollableArea(position, skin));
    for (auto it = objects.begin(); it != objects.end(); ++it)
        result->objects().addObject(*it);
    result->setRecountObjectsBoxes(recountObjectsBoxes);
    return std::move(result);
}

REGISTER_CLASS(ScrollableArea);

}
