/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/findable/IFindable.h>
#include <boost/optional.hpp>
#include <vector>

namespace gamebase { namespace impl {

class IIndex : virtual public IObject {
public:
    virtual void setGameBox(const boost::optional<BoundingBox>& box) = 0;

    virtual void disableFindablesIndex() = 0;
    virtual void update() = 0;
    virtual void insert(int id, IObject* obj) = 0;
    virtual void remove(int id) = 0;
    virtual void clear() = 0;
    
    virtual bool drawablesByBox(const BoundingBox& box, std::vector<Drawable*>&) const = 0;
    virtual std::vector<Drawable*> drawablesByBox(const BoundingBox& box) const
    {
        std::vector<Drawable*> result;
        drawablesByBox(box, result);
        return std::move(result);
    }

    virtual bool findablesByBox(const BoundingBox& box, std::vector<IFindable*>&) const = 0;
    virtual std::vector<IFindable*> findablesByBox(const BoundingBox& box) const
    {
        std::vector<IFindable*> result;
        findablesByBox(box, result);
        return std::move(result);
    }
};

} }
