/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/engine/Drawable.h>
#include <vector>
#include <unordered_map>

namespace gamebase { namespace impl {

class IOrder : virtual public IObject {
public:
    virtual void sort(Drawable** begin, Drawable** end) const = 0;

    virtual void setObjectToID(const std::unordered_map<IObject*, int>& objToID) {}

    void sort(std::vector<Drawable*>& objects) const
    {
        sort(objects.data(), objects.data() + objects.size());
    }

    template <typename ObjType>
    void sort(std::vector<ObjType*>& objects) const
    {
        Drawable** begin = reinterpret_cast<Drawable**>(objects.data());
        auto dst = begin;
        for (auto it = objects.data(); it != objects.data() + objects.size(); ++it, ++dst)
            *dst = dynamic_cast<Drawable*>(*it);
        sort(begin, begin + objects.size());
        auto src = begin;
        for (auto it = objects.data(); it != objects.data() + objects.size(); ++it, ++src)
            *it = dynamic_cast<ObjType*>(*src);
    }
};

} }
