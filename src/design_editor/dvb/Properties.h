/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/drawobj/Label.h>
#include <gamebase/ui/Layout.h>
#include <functional>

namespace gamebase { namespace editor {
class TypePresentation;
class IPropertyPresentation;
class IIndexablePropertyPresentation;

struct Properties {
    Properties();

    int id;
    Label switchButtonLabel;
    Layout layout;
    const TypePresentation* type;
    const IPropertyPresentation* presentationFromParent;
    const IIndexablePropertyPresentation* keyPresentationFromParent;
    std::function<void()> buttonTextUpdater;
    std::shared_ptr<int> collectionSize;
    bool isInline;
};
} }
