/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/findable/IFindable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/impl/geom/BoundingBox.h>

namespace gamebase { namespace impl {

class GAMEBASE_API FindableElement : public virtual IFindable, public Registrable, public ISerializable {
public:
    virtual void connectWith(const IObject* obj) = 0;

    virtual void setBox(const BoundingBox& box) = 0;

    virtual bool lastValue() const = 0;

    virtual void step() = 0;
};

} }
