/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>

namespace gamebase { namespace impl {

class IMovable : virtual public IObject {
public:
    virtual void move(float time) = 0;
};

} }
