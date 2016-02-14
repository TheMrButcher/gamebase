#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/engine/Drawable.h>
#include <vector>

namespace gamebase {

class IIndex : virtual public IObject {
public:
    virtual void setGameBox(const BoundingBox& box) = 0;

    virtual void insert(int id, Drawable* obj) = 0;
    virtual void remove(int id) = 0;
    virtual void clear() = 0;

    virtual std::vector<Drawable*> findByBox(const BoundingBox& box) const = 0;
};

}
