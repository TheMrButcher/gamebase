/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "TreeViewSkin.h"

namespace gamebase { namespace editor {

class SimpleTreeViewSkin : public TreeViewSkin {
public:
    SimpleTreeViewSkin(const std::shared_ptr<impl::IRelativeBox>& box);

    virtual impl::BoundingBox treeBox() const override
    {
        return m_treeBox->get();
    }

    virtual std::shared_ptr<impl::ScrollableArea> createTreeArea() const override;
    virtual std::shared_ptr<impl::ToggleButton> createOpenButton() const override;
    virtual std::shared_ptr<impl::IRelativeBox> createSubtreeBox() const override;
    virtual std::shared_ptr<impl::IRelativeOffset> createOffset() const override;

    virtual void loadResources() override {}
    virtual void drawAt(const Transform2& position) const override {}

    virtual void setBox(const impl::BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        m_treeBox->setParentBox(m_box->get());
    }

    virtual impl::BoundingBox box() const override
    {
        return m_box->get();
    }

private:
    std::shared_ptr<impl::IRelativeBox> m_box;
    std::shared_ptr<impl::IRelativeBox> m_treeBox;
};

} }
