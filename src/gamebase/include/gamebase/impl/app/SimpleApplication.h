/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/app/Application.h>

namespace gamebase { namespace impl {

class GAMEBASE_API SimpleApplication : public Application {
public:
    SimpleApplication();

    virtual void initView() override;
};

} }
