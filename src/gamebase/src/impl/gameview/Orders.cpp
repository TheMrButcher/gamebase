/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameview/SortByYOrder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

namespace {
struct Element {
    Element() {}
    Element(Drawable* drawable, const BoundingBox& box) : drawable(drawable), box(box) {}

    Drawable* drawable;
    BoundingBox box;
};

bool compareBottomY(const Element& e1, const Element& e2)
{
    if (e1.box.bottomLeft.y != e2.box.bottomLeft.y)
        return e1.box.bottomLeft.y > e2.box.bottomLeft.y;
    return e1.box.bottomLeft.x < e2.box.bottomLeft.x;
}
}

void SortByYOrder::sort(Drawable** begin, Drawable** end) const
{
    static std::vector<Element> sorter;
    sorter.clear();
    switch (m_keyType) {
    case GeometryKeyType::Offset:
        for (auto it = begin; it != end; ++it) {
            auto pos = (*it)->drawPosition();
            sorter.push_back(Element(
                *it, BoundingBox(pos ? pos->position().offset : Vec2(0, 0))));
        }
        break;

    case GeometryKeyType::MovedBox:
        for (auto it = begin; it != end; ++it)
            sorter.push_back(Element(*it, (*it)->movedBox()));
        break;

    case GeometryKeyType::TransformedBox:
        for (auto it = begin; it != end; ++it)
            sorter.push_back(Element(*it, (*it)->transformedBox()));
        break;
    }

    std::sort(sorter.begin(), sorter.end(), &compareBottomY);

    auto ptr = begin;
    for (auto it = sorter.begin(); it != sorter.end(); ++it, ++ptr)
        *ptr = it->drawable;
}

void SortByYOrder::serialize(Serializer& s) const
{
    s << "keyType" << m_keyType;
}

std::unique_ptr<IObject> deserializeSortByYOrder(Deserializer& deserializer)
{
    DESERIALIZE(GeometryKeyType::Enum, keyType);
    return std::unique_ptr<IObject>(new SortByYOrder(keyType));
}

REGISTER_CLASS(SortByYOrder);

} }
