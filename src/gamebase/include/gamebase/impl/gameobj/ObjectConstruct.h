/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameobj/AnimatedObjectConstruct.h>
#include <gamebase/impl/gameobj/FindableElement.h>
#include <gamebase/impl/gameobj/SelectableElement.h>

namespace gamebase { namespace impl {

class GAMEBASE_API ObjectConstruct : public AnimatedObjectConstruct, public IFindable, public ISelectable {
public:
    static const int TRANSITION_CHANNEL = 10;

    ObjectConstruct(
        const std::shared_ptr<Drawable>& drawable,
        const std::shared_ptr<PositionElement>& position = nullptr);

    virtual void setFixedBox(float width, float height) override;

    std::shared_ptr<FindableElement> finder() const { return m_findElem; }
    void setFinder(const std::shared_ptr<FindableElement>& findable)
    {
        m_findElem = findable;
        if (m_findElem)
            m_findElem->connectWith(this);
    }

    std::shared_ptr<SelectableElement> selector() const { return m_selectionElem; }
    void setSelector(const std::shared_ptr<SelectableElement>& selectable);

    void setCallback(const std::function<void()>& callback)
    {
        if (m_selectionElem)
            m_selectionElem->setCallback(callback);
    }

    const std::map<SelectionState::Enum, std::string>& transitionAnimations() const { return m_transitionAnimations; }
    void setTransitionAnimation(SelectionState::Enum state, const std::string& animName)
    {
        m_transitionAnimations[state] = animName;
    }

    bool isMouseOn() const
    {
        if (m_selectionElem && m_findElem)
            return m_selectionElem->isMouseOn() && m_findElem->lastValue();
        return false;
    }

    bool isPressed() const
    {
        if (m_selectionElem)
            return m_selectionElem->isPressed();
        return false;
    }

    bool isClicked() const
    {
        if (m_selectionElem)
            return m_selectionElem->isClicked();
        return false;
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

    virtual void drawAt(const Transform2& position) const
    {
        AnimatedObjectConstruct::drawAt(position);
        if (m_selectionElem)
            m_selectionElem->step();
        if (m_findElem)
            m_findElem->step();
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

} }
