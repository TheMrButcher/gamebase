#include <stdafx.h>
#include <gamebase/impl/drawobj/Line.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

Line::Line(const std::shared_ptr<Drawable>& skin)
    : Drawable(this)
    , m_skin(skin)
    , m_inited(false)
    , m_width(0)
    , m_stretchDir(Direction::Horizontal)
{}

void Line::set(const Vec2& p1, const Vec2& p2, float width)
{
    m_p1 = p1;
    m_p2 = p2;
    m_width = width;
    update();
}

void Line::loadResources()
{
    m_skin->loadResources();
    m_inited = true;
}

void Line::setBox(const BoundingBox&)
{
    updateBox();
    m_skin->setBox(m_box);
}

void Line::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject(m_skin);
}

void Line::serialize(Serializer& s) const
{
    s << "skin" << m_skin << "p1" << m_p1 << "p2" << m_p2
        << "width" << m_width << "stretchDir" << m_stretchDir;
}

std::unique_ptr<IObject> deserializeLine(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<Drawable>, skin);
    Vec2 p1, p2;
    if (deserializer.hasMember("p0")) {
        deserializer >> "p0" >> p1 >> "p1" >> p2;
    } else {
        deserializer >> "p1" >> p1 >> "p2" >> p2;
    }
    DESERIALIZE(float, width);
    DESERIALIZE(Direction::Enum, stretchDir);

    std::unique_ptr<Line> result(new Line(skin));
    result->setStretchDir(stretchDir);
    result->set(p1, p2, width);
    return std::move(result);
}

REGISTER_CLASS(Line);

void Line::update()
{
    if (m_inited) {
        updateBox();
        m_skin->setBox(m_box);
        m_skin->loadResources();
    }
}

void Line::updateBox()
{
    auto delta = m_p2 - m_p1;
    auto len = delta.length();
    auto angle = delta.angle();
    if (m_stretchDir == Direction::Vertical)
        angle -= 1.5707963f;
    m_box = m_stretchDir == Direction::Horizontal
        ? Box(len, m_width) : Box(m_width, len);
    m_transform = RotationTransform2(angle) * ShiftTransform2(0.5f * (m_p1 + m_p2));
}

} }
