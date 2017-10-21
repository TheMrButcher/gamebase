/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/app/InputRegister.h>

namespace gamebase { namespace impl {

class IInputProcessor : public virtual IObject {
public:
    virtual void processInput(const InputRegister& input) {}
};

} }
