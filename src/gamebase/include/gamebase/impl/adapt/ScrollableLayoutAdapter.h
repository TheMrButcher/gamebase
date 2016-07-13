#pragma once

#include <gamebase/impl/adapt/LayoutAdapterBase.h>
#include <gamebase/impl/ui/ScrollableArea.h>

namespace gamebase { namespace impl {

class ScrollableLayoutAdapter : public LayoutAdapterBase {
public:
    ScrollableLayoutAdapter(const SmartPointer<ScrollableArea>& area)
        : LayoutAdapterBase(area.get(), area.get(), area.get())
        , m_area(area)
    {}
    
    virtual int addObject(const std::shared_ptr<IObject>& obj) override
    {
        auto& objs = m_area->objects();
        int id = objs.size();
        objs.addObject(obj);
        return id;
    }

    virtual void insertObject(int id, const std::shared_ptr<IObject>& obj) override
    {
        if (static_cast<size_t>(id) == m_area->objects().size()) {
            addObject(obj);
        } else {
            m_area->objects().replaceObject(id, obj);
            m_area->update();
        }
    }

    virtual void removeObject(int id) override
    {
        if (m_area->objects().removeObject(id))
            m_area->update();
    }

    virtual void removeObject(IObject* obj) override
    {
        removeObject(m_area->objects().findObject(obj));
    }

    virtual IObject* getIObject(int id) const override
    {
        const auto& objs = objects();
        if (id < 0 || static_cast<size_t>(id) >= objs.size())
            THROW_EX() << "LinearLayout doesn't contain object with index=" << id;
        return objs[id].get();
    }

    virtual bool hasObject(int id) const override { return id >= 0 && static_cast<size_t>(id) < objects().size(); }
    virtual const std::vector<std::shared_ptr<IObject>>& objects() const override { return m_area->objects().objects(); }
    virtual void clear() override
    {
        m_area->objects().clear();
        m_area->update();
    }

    virtual void setFixedBox(float width, float height) override { m_area->setFixedBox(width, height); }
    virtual void update() override { m_area->update(); }

    virtual SmartPointer<IObject> getInternalObj() const override { return m_area; }

private:
    SmartPointer<ScrollableArea> m_area;
};

} }
