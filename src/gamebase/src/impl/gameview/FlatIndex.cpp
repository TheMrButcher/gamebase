/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameview/FlatIndex.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

namespace {
template <typename Collection>
void removeWithID(Collection& objs, int id)
{
    for (auto it = objs.begin(); it != objs.end(); ++it) {
        if (it->id == id) {
            std::swap(*it, objs.back());
            objs.pop_back();
            break;
        }
    }
}

template <typename T>
bool findByBox(
    GeometryKeyType::Enum geomKeyType,
    const BoundingBox& box,
    const std::vector<FlatIndex::Node<T>>& nodes,
    std::vector<T*>& result)
{
    size_t sizeAtStart = result.size();
    if (geomKeyType == GeometryKeyType::Offset) {
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            if (box.contains(it->box.bottomLeft))
                result.push_back(it->obj);
        }
    } else {
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            if (box.intersects(it->box))
                result.push_back(it->obj);
        }
    }
    return sizeAtStart != result.size();
}

struct CalcOffset {
    void operator()(FlatIndex::Node<Drawable>& node)
    {
        auto pos = node.obj->drawPosition();
        node.box = BoundingBox(pos ? pos->position().offset : Vec2(0, 0));
    }
};

struct CalcMovedBox {
    void operator()(FlatIndex::Node<Drawable>& node)
    {
        node.box = node.obj->movedBox();
    }
};

struct CalcTransformedBox {
    void operator()(FlatIndex::Node<Drawable>& node)
    {
        node.box = node.obj->transformedBox();
    }
};

template <typename Base>
struct CalcFindableAndDrawable : public Base {
    CalcFindableAndDrawable(std::vector<FlatIndex::Node<IFindable>>& findables)
        : it(findables.begin())
        , end(findables.end())
    {}

    void operator()(FlatIndex::Node<Drawable>& node)
    {
        Base::operator()(node);
        if (it != end && it->id == node.id) {
            it->box = node.box;
            ++it;
        }
    }

    typename std::vector<FlatIndex::Node<IFindable>>::iterator it;
    typename std::vector<FlatIndex::Node<IFindable>>::iterator end;
};

template <typename Calc>
void update(std::vector<FlatIndex::Node<Drawable>>& drawables, Calc& calc)
{
    for (auto it = drawables.begin(); it != drawables.end(); ++it)
        calc(*it);
}

template <typename Calc>
void updateImpl(
    std::vector<FlatIndex::Node<Drawable>>& drawables,
    std::vector<FlatIndex::Node<IFindable>>& findables,
    bool needFindables)
{
    if (needFindables) {
        CalcFindableAndDrawable<Calc> calc(findables);
        update(drawables, calc);
    } else {
        Calc calc;
        update(drawables, calc);
    }
}
}

void FlatIndex::update()
{
    switch (m_keyType) {
    case GeometryKeyType::Offset:
        updateImpl<CalcOffset>(m_objs, m_findables, m_needFindables);
        break;

    case GeometryKeyType::MovedBox:
        updateImpl<CalcMovedBox>(m_objs, m_findables, m_needFindables);
        break;

    case GeometryKeyType::TransformedBox:
        updateImpl<CalcTransformedBox>(m_objs, m_findables, m_needFindables);
        break;
    }
}

void FlatIndex::insert(int id, IObject* obj)
{
    auto drawable = dynamic_cast<Drawable*>(obj);
    if (!drawable)
        return;
    m_objs.push_back(Node<Drawable>(id, drawable));
    if (m_needFindables) {
        auto findable = dynamic_cast<IFindable*>(obj);
        if (findable)
            m_findables.push_back(Node<IFindable>(id, findable));
    }
}

void FlatIndex::remove(int id)
{
    removeWithID(m_objs, id);
    if (m_needFindables)
        removeWithID(m_findables, id);
}

bool FlatIndex::drawablesByBox(
    const BoundingBox& box, std::vector<Drawable*>& drawables) const
{
    return findByBox(m_keyType, box, m_objs, drawables);
}

bool FlatIndex::findablesByBox(
    const BoundingBox& box, std::vector<IFindable*>& findables) const
{
    return findByBox(m_keyType, box, m_findables, findables);
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

} }
