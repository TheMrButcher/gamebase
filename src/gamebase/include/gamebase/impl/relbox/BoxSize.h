/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

namespace gamebase { namespace impl {

struct BoxSize {
    enum Type {
        Width,
        Height,
        Min,
        Max
    };
};

} }