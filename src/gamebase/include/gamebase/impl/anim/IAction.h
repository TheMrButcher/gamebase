/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/reg/PropertiesRegister.h>

namespace gamebase { namespace impl {

class IAction : virtual public IObject {
public:
    virtual void load(const PropertiesRegister& props) = 0;

    virtual void exec() = 0;
};

} }
