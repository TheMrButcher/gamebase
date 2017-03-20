/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameview/SortByIDOrder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

namespace {
struct Element {
    Element() {}
    Element(Drawable* drawable, int id) : drawable(drawable), id(id) {}

    Drawable* drawable;
    int id;
};

bool compareID(const Element& e1, const Element& e2)
{
    return e1.id < e2.id;
}
}

void SortByIDOrder::sort(Drawable** begin, Drawable** end) const
{
    static std::vector<Element> sorter;
    sorter.clear();

    for (auto it = begin; it != end; ++it) {
        auto drawable = *it;
        sorter.push_back(Element(drawable, m_objToID->find(drawable)->second));
    }
    std::sort(sorter.begin(), sorter.end(), &compareID);

    auto ptr = begin;
    for (auto it = sorter.begin(); it != sorter.end(); ++it, ++ptr)
        *ptr = it->drawable;
}

std::unique_ptr<IObject> deserializeSortByIDOrder(Deserializer& deserializer)
{
    return std::unique_ptr<IObject>(new SortByIDOrder());
}

REGISTER_CLASS(SortByIDOrder);

} }
