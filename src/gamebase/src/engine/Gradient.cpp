#include <stdafx.h>
#include <gamebase/engine/Gradient.h>
#include <gamebase/engine/FixedBox.h>
#include <gamebase/graphics/MultiColorProgram.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

void Gradient::setFixedBox(float width, float height)
{
    m_box = std::make_shared<FixedBox>(width, height);
}

void Gradient::loadResources()
{
    m_buffers = createGradientBuffers(m_box->get(), m_color1, m_color2, m_dir);
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

}
