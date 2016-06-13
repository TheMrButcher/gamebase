#pragma once

#include <gamebase/impl/adapt/LayoutAdapterBase.h>
#include <gamebase/impl/ui/LinearLayout.h>

namespace gamebase { namespace impl {

class LinearLayoutAdapter : public LayoutAdapterBase {
public:
    LinearLayoutAdapter(const std::shared_ptr<LinearLayout>& layout)
        : LayoutAdapterBase(layout.get(), layout.get())
        , m_layout(layout)
    {}

    virtual void addObject(const std::shared_ptr<IObject>& obj) override { m_layout->addObject(obj); }
    virtual const std::vector<std::shared_ptr<IObject>>& objects() const { return m_layout->objects(); }
    virtual void clear() { m_layout->clear(); }
    virtual void setFixedBox(float width, float height) { m_layout->setFixedBox(width, height); }
    virtual void update() { m_layout->update(); }

    virtual std::shared_ptr<IObject> getInternalObj() const override { return m_layout; }

private:
    std::shared_ptr<LinearLayout> m_layout;
};

} }
