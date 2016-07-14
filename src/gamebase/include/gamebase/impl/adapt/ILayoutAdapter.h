/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/pubhelp/SmartPointer.h>
#include <gamebase/geom/BoundingBox.h>
#include <vector>
#include <memory>

namespace gamebase { namespace impl {

class ILayoutAdapter {
public:
    virtual ~ILayoutAdapter() {}

    virtual int addObject(const std::shared_ptr<IObject>& obj) = 0;
    virtual void insertObject(int id, const std::shared_ptr<IObject>& obj) = 0;
    virtual void removeObject(int id) = 0;
    virtual void removeObject(IObject* obj) = 0;
    virtual IObject* getIObject(int id) const = 0;
    virtual bool hasObject(int id) const = 0;
    virtual bool hasObject(IObject* obj) const = 0;
    virtual const std::vector<std::shared_ptr<IObject>>& objects() const = 0;
    virtual void clear() = 0;
    virtual void setFixedBox(float width, float height) = 0;
    virtual void update() = 0;

    virtual IObject* getAbstractChild(const std::string& name) const = 0;

    virtual bool isVisible() const = 0;
    virtual void setVisible(bool value) = 0;

    virtual Vec2 getOffset() const = 0;
    virtual void setOffset(const Vec2& v) = 0;
    virtual BoundingBox box() const = 0;
    virtual BoundingBox movedBox() const = 0;

    virtual SmartPointer<IObject> getInternalObj() const = 0;
};

} }
