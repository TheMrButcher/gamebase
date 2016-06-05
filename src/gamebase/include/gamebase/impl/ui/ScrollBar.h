#pragma once

#include <gamebase/impl/skin/base/ScrollBarSkin.h>
#include <gamebase/impl/tools/ObjectsCollection.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API ScrollBar : public OffsettedPosition, public Drawable,
    public IFindable, public Registrable, public ISerializable {
public:
    ScrollBar(
        const std::shared_ptr<ScrollBarSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setControlledValue(const std::shared_ptr<FloatValue>& controlledValue)
    {
        m_controlledValue = controlledValue;
        update();
    }

    void setRange(float minVal, float maxVal)
    {
        if (minVal > maxVal)
            THROW_EX() << "Min value (" << minVal << ") is bigger than maxVal(" << maxVal << ")";
        m_minVal = minVal;
        m_maxVal = maxVal;
        update();
    }

    void setVisibleZoneSize(float size)
    {
        if (size < 0.0f)
            THROW_EX() << "Wrong visible zone size: " << size;
        m_visibleZoneSize = size;
        update();
    }

    void setParams(float minVal, float maxVal, float visibleZoneSize)
    {
        if (minVal > maxVal)
            THROW_EX() << "Min value (" << minVal << ") is bigger than maxVal(" << maxVal << ")";
        if (visibleZoneSize < 0.0f)
            THROW_EX() << "Wrong visible zone size: " << visibleZoneSize;
        m_minVal = minVal;
        m_maxVal = maxVal;
        m_visibleZoneSize = visibleZoneSize;
        update();
    }

    void setAssociatedSelectable(ISelectable* selectable)
    {
        m_collection.setAssociatedSelectable(selectable);
    }

    void move(int numOfSteps);
    
    virtual void loadResources() override;

    virtual void drawAt(const Transform2& position) const override
    {
        m_skin->draw(position);
        m_collection.draw(position);
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override;

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override
    {
        if (!isVisible())
            return nullptr;
        return m_collection.findChildByPoint(position().inversed() * point);
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    void decrease();
    void increase();
    void update();
    void dragByPixels(float startValue, float offsetInPixels);
    void step(float value);

    std::shared_ptr<ScrollBarSkin> m_skin;
    ObjectsCollection m_collection;
    bool m_inited;
    std::shared_ptr<FloatValue> m_controlledValue;
    float m_minVal;
    float m_maxVal;
    float m_visibleZoneSize;

    class DragBarMovement;
    
    std::shared_ptr<FixedOffset> m_dragBarOffset;
    std::shared_ptr<DragBarMovement> m_dragBarCallback;
    std::shared_ptr<ScrollDragBar> m_dragBar;
    float (BoundingBox::*m_sizeFunc)() const;
};

} }
