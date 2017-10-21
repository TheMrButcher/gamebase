/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/findable/IFindable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API Backgrounded : public OffsettedPosition, public Drawable,
    public ISerializable, public Registrable, public IFindable {
public:
    Backgrounded(const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setMainObject(const std::shared_ptr<IObject>& obj);
    void setBackground(const std::shared_ptr<IObject>& obj);

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override { return m_curBox; }

    virtual void registerObject(PropertiesRegisterBuilder*) override;
    virtual void serialize(Serializer& serializer) const override;

protected:
    BoundingBox m_curBox;

    std::shared_ptr<IObject> m_mainObj;
    Drawable* m_mainDrawable;
    IFindable* m_mainFindable;

    std::shared_ptr<IObject> m_backgroundObj;
    Drawable* m_backgroundDrawable;
};

} }
