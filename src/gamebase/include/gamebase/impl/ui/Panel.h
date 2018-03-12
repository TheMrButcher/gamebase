/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/skin/base/PanelSkin.h>
#include <gamebase/impl/tools/ObjectsCollection.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API Panel : public OffsettedPosition, public Drawable,
    public Registrable, public IFindable, public Selectable, public ISerializable {
public:
    Panel(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<PanelSkin>& skin);

    void addObject(const std::shared_ptr<IObject>& obj)
    {
        m_objects.addObject(obj);
    }

    virtual void setSelectionState(SelectionState::Enum state) override {}

    void setAssociatedSelectable(ISelectable* selectable) override
    {
        Selectable::setAssociatedSelectable(selectable);
        m_objects.setAssociatedSelectable(selectable);
    }

    void setCloseCallback(const std::function<void()>& closeCallback)
    {
        m_closeCallback = closeCallback;
    }
    
    void resetPosition();
    void close();

    ObjectsCollection& objects() { return m_objects; }

    virtual Transform2 position() const override;
    virtual bool isSelectableByPoint(const Vec2& point) const override;
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;
    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override;

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<PanelSkin> m_skin;
    class DragOffset;
    std::shared_ptr<DragOffset> m_dragOffset;
    ObjectsCollection m_sysObjects;
    ObjectsCollection m_objects;
    std::function<void()> m_closeCallback;
};

} }
