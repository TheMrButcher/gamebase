#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/engine/Drawable.h>
#include <vector>

namespace gamebase {

class IOrder : virtual public IObject {
public:
    virtual void sort(Drawable** begin, Drawable** end) = 0;

    template <typename ObjType>
    void sort(std::vector<ObjType*>& objects)
    {
        Drawable** begin = reinterpret_cast<Drawable**>(objects.data());
        sort(begin, begin + objects.size());
    }
};

}
