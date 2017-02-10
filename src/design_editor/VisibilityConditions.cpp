/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "VisibilityConditions.h"
#include "Settings.h"
#include "PropertyPresentation.h"
#include <dvb/Properties.h>

namespace gamebase { namespace editor {
namespace {
class AlwaysHide : public IVisibilityCondition {
public:
    AlwaysHide() : IVisibilityCondition("alwaysHide") {}

    virtual bool allowShow(const SharedContext&, const std::shared_ptr<Properties>&) const { return false; }
};

class ShowInComplexBoxMode : public IVisibilityCondition {
public:
    ShowInComplexBoxMode() : IVisibilityCondition("showInComplexBoxMode") {}

    virtual bool allowShow(const SharedContext&, const std::shared_ptr<Properties>&) const { return settings::isComplexBoxMode; }
};

class TagChecker : public IVisibilityCondition {
public:
    TagChecker(const std::string& name, const std::string& tag)
        : IVisibilityCondition(name)
        , m_tag(tag)
    {}

    virtual bool allowShow(const SharedContext&, const std::shared_ptr<Properties>& props) const
    {
        auto propertyPresentation = props->presentationFromParent;
        if (auto objectPresentation = dynamic_cast<const ObjectPresentation*>(propertyPresentation)) {
            if (objectPresentation->tags.count(m_tag) != 0)
                return false;
        }
        return true;
    }

private:
    std::string m_tag;
};

class VisibilityConditionsRegistrar {
public:
    VisibilityConditionsRegistrar()
    {
        VisibilityConditions::instance().add(std::make_shared<AlwaysHide>());
        VisibilityConditions::instance().add(std::make_shared<ShowInComplexBoxMode>());
        VisibilityConditions::instance().add(std::make_shared<TagChecker>("checkPosition", "hidePosition"));
        VisibilityConditions::instance().add(std::make_shared<TagChecker>("checkVisibility", "hideVisibility"));
        VisibilityConditions::instance().add(std::make_shared<TagChecker>("checkName", "hideName"));
    }
};
const VisibilityConditionsRegistrar vcr;
}
} }
