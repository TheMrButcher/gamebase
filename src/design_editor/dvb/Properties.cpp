/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "Properties.h"

namespace gamebase { namespace editor {
Properties::Properties()
    : id(-1)
    , layout(nullptr)
    , type(nullptr)
    , presentationFromParent(nullptr)
    , keyPresentationFromParent(nullptr)
    , isInline(false)
    , isHiddenLevel(false)
{}
} }
