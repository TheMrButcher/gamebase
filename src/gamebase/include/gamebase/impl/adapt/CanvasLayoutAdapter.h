/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/adapt/LayoutAdapterBase.h>
#include <gamebase/impl/ui/CanvasLayout.h>

namespace gamebase { namespace impl {

class CanvasLayoutAdapter : public LayoutAdapterBase {
public:
    CanvasLayoutAdapter(const SmartPointer<CanvasLayout>& layout)
        : LayoutAdapterBase(layout.get(), layout.get(), layout.get())
        , m_layout(layout)
    {}

    virtual int addObject(const std::shared_ptr<IObject>& obj) override { return m_layout->addObject(obj); }
    virtual void insertObject(int id, const std::shared_ptr<IObject>& obj) override { m_layout->insertObject(id, obj); }
    virtual void removeObject(int id) override { m_layout->removeObject(id); }
    virtual void removeObject(IObject* obj) override { m_layout->removeObject(obj); }
    virtual IObject* getIObject(int id) const override { return m_layout->getIObject(id); }
    virtual bool hasObject(int id) const override { return m_layout->hasObject(id); }
    virtual const std::vector<std::shared_ptr<IObject>>& objects() const override { return m_layout->objectsAsList(); }
    virtual void clear() override { m_layout->clear(); }
    virtual void setFixedBox(float width, float height) override { m_layout->setFixedBox(width, height); }
    virtual void update() override { m_layout->update(); }

    virtual SmartPointer<IObject> getInternalObj() const override { return m_layout; }

private:
    SmartPointer<CanvasLayout> m_layout;
};

} }
