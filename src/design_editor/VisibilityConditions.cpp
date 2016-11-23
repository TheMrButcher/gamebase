/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "VisibilityConditions.h"
#include "Settings.h"

namespace gamebase { namespace editor {
namespace {
class AlwaysHide : public IVisibilityCondition {
public:
    AlwaysHide() : IVisibilityCondition("alwaysHide") {}

    virtual bool allowShow(const SharedContext&, int) const { return false; }
};

class ShowInComplexBoxMode : public IVisibilityCondition {
public:
    ShowInComplexBoxMode() : IVisibilityCondition("showInComplexBoxMode") {}

    virtual bool allowShow(const SharedContext&, int) const { return settings::isComplexBoxMode; }
};

class VisibilityConditionsRegistrar {
public:
    VisibilityConditionsRegistrar()
    {
        VisibilityConditions::instance().add(std::make_shared<AlwaysHide>());
        VisibilityConditions::instance().add(std::make_shared<ShowInComplexBoxMode>());
    }
};
const VisibilityConditionsRegistrar vcr;
}
} }
