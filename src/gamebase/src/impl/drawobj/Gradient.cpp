#include <stdafx.h>
#include <gamebase/impl/drawobj/Gradient.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/graphics/MultiColorProgram.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void Gradient::setFixedBox(float width, float height)
{
    auto box = std::make_shared<FixedBox>(width, height);
    if (m_box->isValid())
        box->checkInited();
    m_box = box;
    reload();
}

void Gradient::drawAt(const Transform2& position) const
{
    if (m_color1.a == 0 && m_color2.a == 0)
        return;
    const MultiColorProgram& program = multiColorProgram();
    program.transform = position;
    program.draw(m_buffers.vbo, m_buffers.ibo);
}

void Gradient::serialize(Serializer& s) const
{
    s << "box" << m_box << "position" << m_offset
        << "color1" << m_color1 << "color2" << m_color2
        << "direction" << m_dir;
}

std::unique_ptr<IObject> deserializeGradient(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(Color, color1);
    DESERIALIZE(Color, color2);
    DESERIALIZE(Direction::Enum, direction);
    std::unique_ptr<Gradient> result(new Gradient(box, position));
    result->setColor1(color1);
    result->setColor2(color2);
    result->setDirection(direction);
    return std::move(result);
}

REGISTER_CLASS(Gradient);

void Gradient::reload()
{
    if (m_box->isValid())
        m_buffers = createGradientBuffers(m_box->get(), m_color1, m_color2, m_dir);
}

} }
