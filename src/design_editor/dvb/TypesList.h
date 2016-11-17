/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/ui/ComboBox.h>

namespace gamebase { namespace editor {
class TypePresentation;

struct TypesList {
    ComboBox comboBox;
    std::vector<const TypePresentation*> types;
    size_t indexInLayout;
};
} }
