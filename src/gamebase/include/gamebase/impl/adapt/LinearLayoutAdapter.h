#pragma once

#include <gamebase/impl/adapt/LayoutAdapterBase.h>
#include <gamebase/impl/ui/LinearLayout.h>

namespace gamebase { namespace impl {

class LinearLayoutAdapter : public LayoutAdapterBase {
public:
    LinearLayoutAdapter(const SmartPointer<LinearLayout>& layout)
        : LayoutAdapterBase(layout.get(), layout.get(), layout.get())
        , m_layout(layout)
    {}
    
    virtual int addObject(const std::shared_ptr<IObject>& obj) override { return m_layout->addObject(obj); }
    virtual void insertObject(int id, const std::shared_ptr<IObject>& obj) override { m_layout->insertObject(id, obj); }
    virtual void removeObject(int id) override { m_layout->removeObject(id); }
    virtual void removeObject(IObject* obj) override { m_layout->removeObject(obj); }
    virtual IObject* getIObject(int id) const override
    {
        const auto& objs = objects();
        if (id < 0 || static_cast<size_t>(id) >= objs.size())
            THROW_EX() << "LinearLayout doesn't contain object with index=" << id;
        return objs[id].get();
    }

    virtual bool hasObject(int id) const override { return id >= 0 && static_cast<size_t>(id) < objects().size(); }
    virtual const std::vector<std::shared_ptr<IObject>>& objects() const override { return m_layout->objects(); }
    virtual void clear() override { m_layout->clear(); }
    virtual void setFixedBox(float width, float height) override { m_layout->setFixedBox(width, height); }
    virtual void update() override { m_layout->update(); }

    virtual SmartPointer<IObject> getInternalObj() const override { return m_layout; }

private:
    SmartPointer<LinearLayout> m_layout;
};

} }
