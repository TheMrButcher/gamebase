#pragma once

#include <gamebase/engine/ScrollBarSkin.h>
#include <gamebase/engine/ObjectsCollection.h>

namespace gamebase {

class GAMEBASE_API ScrollBar : public OffsettedPosition, public IFindable, public IDrawable {
public:
    ScrollBar(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<ScrollBarSkin>& skin,
        const std::shared_ptr<FloatValue>& controlledValue);

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

    void setStepSize(float step)
    {
        m_step = step;
    }

    void setAssociatedSelectable(ISelectable* selectable)
    {
        m_collection.setAssociatedSelectable(selectable);
    }
    
    virtual void loadResources() override
    {
        m_skin->loadResources();
        m_collection.loadResources();
    }

    virtual void draw(const Transform2& globalPosition) const override
    {
        if (m_visible) {
            auto fullPosition = transform() * globalPosition;
            m_skin->draw(fullPosition);
            m_collection.draw(fullPosition);
        }
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override;

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override
    {
        if (m_visible)
            return m_collection.find(point, transform() * globalPosition);
        return nullptr;
    }

private:
    void decrease();
    void increase();
    void update();
    void dragByPixels(float startValue, float offsetInPixels);
    void step(float value);

    std::shared_ptr<ScrollBarSkin> m_skin;
    bool m_visible;
    ObjectsCollection m_collection;
    std::shared_ptr<FloatValue> m_controlledValue;
    float m_minVal;
    float m_maxVal;
    float m_visibleZoneSize;
    float m_step;

    class DragBarMovement;
    
    std::shared_ptr<FixedOffset> m_dragBarOffset;
    std::shared_ptr<DragBarMovement> m_dragBarCallback;
    std::shared_ptr<ScrollDragBar> m_dragBar;
    float (BoundingBox::*m_sizeFunc)() const;
};

}
