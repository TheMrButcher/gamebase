#pragma once

#include <gamebase/impl/adapt/LayoutAdapterBase.h>
#include <gamebase/impl/ui/SelectingWidget.h>

namespace gamebase { namespace impl {

class SelectingLayoutAdapter : public LayoutAdapterBase {
public:
    SelectingLayoutAdapter(const SmartPointer<SelectingWidget>& layout)
        : LayoutAdapterBase(layout.get(), layout.get(), layout.get())
        , m_layout(layout)
    {}

    virtual int addObject(const std::shared_ptr<IObject>& obj) override { return m_layout->addObject(obj); }
    virtual void insertObject(int id, const std::shared_ptr<IObject>& obj) override { m_layout->insertObject(id, obj); }
    virtual void removeObject(int id) override { m_layout->removeObject(id); }
    virtual void removeObject(IObject* obj) override { m_layout->removeObject(obj); }
    virtual IObject* getIObject(int id) const override { return m_layout->getIObject(id); }
    virtual bool hasObject(int id) const { return m_layout->hasObject(id); }
    virtual bool hasObject(IObject* obj) const { return m_layout->hasObject(obj); }
    virtual const std::vector<std::shared_ptr<IObject>>& objects() const { return m_layout->objectsAsList(); }
    virtual void clear() { m_layout->clear(); }
    virtual void setFixedBox(float width, float height) { m_layout->setFixedBox(width, height); }
    virtual void update()
    {
        m_layout->markAllNeedReload();
        m_layout->select(m_layout->selected());
    }

    virtual SmartPointer<IObject> getInternalObj() const override { return m_layout; }

private:
    SmartPointer<SelectingWidget> m_layout;
};

} }
