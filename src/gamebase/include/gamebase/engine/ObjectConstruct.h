#pragma once

#include <gamebase/engine/AnimatedObjectConstruct.h>
#include <gamebase/engine/FindableElement.h>
#include <gamebase/engine/SelectableElement.h>

namespace gamebase {

class GAMEBASE_API ObjectConstruct : public AnimatedObjectConstruct, public IFindable, public ISelectable {
public:
    static const int TRANSITION_CHANNEL = 10;

    ObjectConstruct(
        const std::shared_ptr<IDrawable>& drawable,
        const std::shared_ptr<PositionElement>& position = nullptr);

    void setFinder(const std::shared_ptr<FindableElement>& findable)
    {
        m_findElem = findable;
        if (m_findElem)
            m_findElem->connectWith(this);
    }

    void setSelector(const std::shared_ptr<SelectableElement>& selectable);

    void setCallback(const std::function<void()>& callback)
    {
        if (m_selectionElem)
            m_selectionElem->setCallback(callback);
    }

    void setTransitionAnimation(SelectionState::Enum state, const std::string& animName)
    {
        m_transitionAnimations[state] = animName;
    }

    virtual bool isSelectableByPoint(const Vec2& point) const override
    {
        if (!m_findElem)
            return false;
        return m_findElem->isSelectableByPoint(point);
    }

    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override
    {
        if (!m_findElem)
            return nullptr;
        return m_findElem->findChildByPoint(point);
    }

    virtual SelectionState::Enum selectionState() const override
    {
        if (!m_selectionElem)
            return SelectionState::Disabled;
        return m_selectionElem->selectionState();
    }

    virtual void setSelectionState(SelectionState::Enum state) override
    {
        if (m_selectionElem)
            m_selectionElem->setSelectionState(state);
    }

    virtual ISelectable* associatedSelectable() const override
    {
        if (!m_selectionElem)
            return nullptr;
        return m_selectionElem->associatedSelectable();
    }

    virtual void setAssociatedSelectable(ISelectable* selectable) override
    {
        if (m_selectionElem)
            m_selectionElem->setAssociatedSelectable(selectable);
    }

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        AnimatedObjectConstruct::setBox(allowedBox);
        if (m_findElem)
            m_findElem->setBox(box());
    }

    virtual void registerObject(PropertiesRegisterBuilder*) override;
    virtual void serialize(Serializer& serializer) const override;

private:
    void runTransitionAnimation(SelectionState::Enum state);

    std::shared_ptr<FindableElement> m_findElem;
    std::shared_ptr<SelectableElement> m_selectionElem;
    std::map<SelectionState::Enum, std::string> m_transitionAnimations;
};

}
