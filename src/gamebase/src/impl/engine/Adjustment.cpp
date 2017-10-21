/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/engine/Adjustment.h>

namespace gamebase { namespace impl {

BoundingBox adjust(
    const BoundingBox& areaBox,
    const BoundingBox& contentBox,
    Adjustment::Enum adjustment)
{
    BoundingBox box;
    if (adjustment == Adjustment::None) {
        box = areaBox;
    } else {
        box = contentBox;
        if (!box.isValid() || adjustment == Adjustment::ToFitContentAndArea)
            box.add(areaBox);
    }
    return box;
}

} }
