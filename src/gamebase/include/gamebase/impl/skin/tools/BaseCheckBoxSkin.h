/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/skin/base/CheckBoxSkin.h>

namespace gamebase { namespace impl {

class GAMEBASE_API BaseCheckBoxSkin : public CheckBoxSkin {
public:
    BaseCheckBoxSkin(const std::shared_ptr<IRelativeBox>& box);

    virtual std::shared_ptr<IRelativeGeometry> geometry() const override { return m_geom; }
    virtual void setChecked(bool status) override { m_checked = status; }
    virtual void setSelectionState(SelectionState::Enum) override {}
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override
    {
        return m_box->get();
    }
    virtual void registerObject(PropertiesRegisterBuilder* builder) override {}

protected:
    virtual void drawBackground(const Transform2& position) const = 0;
    virtual void drawCheck(const Transform2& position) const = 0;
    virtual void setContentBox(const BoundingBox& box) = 0;

    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IRelativeGeometry> m_geom;
    bool m_checked;
};

} }
