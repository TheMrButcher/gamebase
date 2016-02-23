#pragma once

#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/engine/IFindable.h>

namespace gamebase {

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

}
