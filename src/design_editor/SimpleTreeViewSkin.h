#pragma once

#include "TreeViewSkin.h"

namespace gamebase { namespace editor {

class SimpleTreeViewSkin : public TreeViewSkin {
public:
    SimpleTreeViewSkin(const std::shared_ptr<IRelativeBox>& box);

    virtual BoundingBox treeBox() const override
    {
        return m_treeBox->get();
    }

    virtual std::shared_ptr<ScrollableArea> createTreeArea() const override;
    virtual std::shared_ptr<ToggleButton> createOpenButton() const override;
    virtual std::shared_ptr<IRelativeBox> createSubtreeBox() const override;
    virtual std::shared_ptr<IRelativeOffset> createOffset() const override;

    virtual void loadResources() override {}
    virtual void drawAt(const Transform2& position) const override {}

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        m_treeBox->setParentBox(m_box->get());
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

private:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IRelativeBox> m_treeBox;
};

} }
