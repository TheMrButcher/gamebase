#include <stdafx.h>
#include <gamebase/engine/FlatIndex.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

void FlatIndex::update()
{
    switch (m_keyType) {
    case GeometryKeyType::Offset:
        for (auto it = m_objs.begin(); it != m_objs.end(); ++it) {
            auto pos = it->obj->drawPosition();
            it->box = BoundingBox(pos ? pos->position().offset : Vec2(0, 0));
        }
        break;

    case GeometryKeyType::MovedBox:
        for (auto it = m_objs.begin(); it != m_objs.end(); ++it)
            it->box = BoundingBox(it->obj->movedBox());
        break;

    case GeometryKeyType::TransformedBox:
        for (auto it = m_objs.begin(); it != m_objs.end(); ++it)
            it->box = BoundingBox(it->obj->transformedBox());
        break;
    }
}

void FlatIndex::insert(int id, Drawable* obj)
{
    m_objs.push_back(Node(id, obj));
}

void FlatIndex::remove(int id)
{
    for (auto it = m_objs.begin(); it != m_objs.end(); ++it) {
        if (it->id == id) {
            std::swap(*it, m_objs.back());
            m_objs.pop_back();
        }
    }
}

std::vector<Drawable*> FlatIndex::findByBox(const BoundingBox& box) const
{
    std::vector<Drawable*> result;
    result.reserve(m_objs.size());
    if (m_keyType == GeometryKeyType::Offset) {
        for (auto it = m_objs.begin(); it != m_objs.end(); ++it) {
            if (box.contains(it->box.bottomLeft))
                result.push_back(it->obj);
        }
    } else {
        for (auto it = m_objs.begin(); it != m_objs.end(); ++it) {
            if (box.intersects(it->box))
                result.push_back(it->obj);
        }
    }
    return std::move(result);
}

void FlatIndex::serialize(Serializer& s) const
{
    s << "keyType" << m_keyType;
}

std::unique_ptr<IObject> deserializeFlatIndex(Deserializer& deserializer)
{
    DESERIALIZE(GeometryKeyType::Enum, keyType);
    return std::unique_ptr<IObject>(new FlatIndex(keyType));
}

REGISTER_CLASS(FlatIndex);

}
