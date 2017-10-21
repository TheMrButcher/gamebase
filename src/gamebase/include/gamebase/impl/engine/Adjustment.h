/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/geom/BoundingBox.h>

namespace gamebase { namespace impl {

struct Adjustment {
    enum Enum {
        None,
        ToFitContent,
        ToFitContentAndArea
    };
};

BoundingBox adjust(
    const BoundingBox& areaBox,
    const BoundingBox& contentBox,
    Adjustment::Enum adjustment);

} }
