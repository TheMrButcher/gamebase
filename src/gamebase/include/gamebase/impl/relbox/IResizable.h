/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

namespace gamebase { namespace impl {

class IResizable {
public:
    virtual ~IResizable() {}

    virtual void setFixedBox(float width, float height) = 0;
};

} }
