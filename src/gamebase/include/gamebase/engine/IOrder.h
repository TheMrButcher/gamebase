#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/engine/Drawable.h>
#include <vector>

namespace gamebase {

class IOrder : virtual public IObject {
public:
    virtual void sort(Drawable** begin, Drawable** end) const = 0;

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

}