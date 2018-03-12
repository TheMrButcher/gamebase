/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

namespace gamebase { namespace editor {
struct ObjType {
    enum Enum {
        Unknown,
        PrimitiveArray,
        Object,
        Array,
        Map,
        MapElement,
        FictiveObject
    };
};
} }
