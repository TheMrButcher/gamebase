/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/base/ScrollableAreaSkin.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/tools/ObjectsCollection.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API CommonScrollableAreaSkin : public ScrollableAreaSkin, public ISerializable {
public:
    CommonScrollableAreaSkin(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeBox>& areaBox);

    void addElement(const std::shared_ptr<IObject>& element)
    {
        m_skinElements.addObject(element);
    }

    void setAreaWithoutScrollBarsBox(
        const std::shared_ptr<IRelativeBox>& box)
    {
        m_areaWithoutScrollBarsBox = box;
    }

    void setScrollBarSkin(
        const std::shared_ptr<ScrollBarSkin>& skin,
        Direction::Enum direction,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    virtual std::shared_ptr<ScrollBar> createScrollBar(
        const std::shared_ptr<FloatValue>& controlledValue,
        Direction::Enum direction) const override;

    virtual BoundingBox areaBox() const override { return m_curAreaBox; }
    virtual void setSize(float width, float height) override;
    virtual void setFixedBox(float width, float height) override;

    virtual void loadResources() override
    {
        m_skinElements.loadResources();
    }

    virtual void drawAt(const Transform2& position) const override
    {
        m_skinElements.draw(position);
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override;

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;

    virtual void serialize(Serializer& s) const override;

protected:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IRelativeBox> m_areaBox;
    std::shared_ptr<IRelativeBox> m_areaWithoutScrollBarsBox;
    BoundingBox m_curAreaBox;

    std::shared_ptr<ScrollBarSkin> m_scrollBarSkins[2];
    std::shared_ptr<IRelativeOffset> m_scrollBarPositions[2];

    ObjectsCollection m_skinElements;
};

} }
