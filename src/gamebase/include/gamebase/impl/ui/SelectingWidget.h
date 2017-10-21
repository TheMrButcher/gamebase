/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/tools/ObjectsSelector.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/engine/Adjustment.h>
#include <set>

namespace gamebase { namespace impl {

class GAMEBASE_API SelectingWidget : public ObjectsSelector, public OffsettedPosition {
public:
    SelectingWidget(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);
    
    Adjustment::Enum adjustment() const { return m_adjustment; }
    void setAdjustment(Adjustment::Enum value) { m_adjustment = value; }
    
    void markAllNeedReload() { m_initedObjects.clear(); }
    void markNeedReload(int id) { m_initedObjects.erase(id); }
    int addObject(const std::shared_ptr<IObject>& object);
    virtual void insertObject(int id, const std::shared_ptr<IObject>& object) override;
    virtual void removeObject(int id) override;
    void removeObject(IObject* obj);
    bool hasObject(int id) const { return m_objects.find(id) != m_objects.end(); }
    bool hasObject(IObject* obj) const;
    IObject* getIObject(int id) const;
    virtual void clear() override;
    virtual void select(int id) override;
    void setFixedBox(float width, float height);
    const std::vector<std::shared_ptr<IObject>>& objectsAsList() const;

    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual void loadResources() override;
    virtual BoundingBox box() const override { return m_curBox; }

    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IRelativeBox> m_box;
    Adjustment::Enum m_adjustment;
    BoundingBox m_parentBox;
    BoundingBox m_curBox;
    std::set<int> m_initedObjects;
    int m_nextID;
    mutable std::vector<std::shared_ptr<IObject>> m_cachedObjs;
};

typedef SelectingWidget Selector;

} }
