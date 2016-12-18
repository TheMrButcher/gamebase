/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "LinearLayout.h"
#include "SimpleRelativeValue.h"
#include <gamebase/impl/skin/impl/HorizontalLayoutSkin.h>
#include <gamebase/impl/skin/impl/VerticalLayoutSkin.h>

namespace gamebase { namespace editor {

namespace {
void serializeOffset(const impl::LinearLayout* layout, impl::Serializer& s)
{
    auto offset = layout->offset<impl::IRelativeOffset>();
    if (!offset) {
        s   << "position" << std::shared_ptr<impl::IRelativeOffset>();
    } else {
        s   << "position" << *offset;
    }
}
}

void serializeLinearLayout(const impl::IObject* obj, impl::Serializer& s)
{
    if (auto layout = dynamic_cast<const impl::LinearLayout*>(obj)) {
        auto skin = layout->skin();
        auto dir = layout->direction();
        if (dir == impl::Direction::Horizontal) {
            auto horSkin = dynamic_cast<impl::HorizontalLayoutSkin*>(skin);
            if (!horSkin)
                THROW_EX() << "Unexpected type of LinearLayout skin";
            s   << "box" << horSkin->relativeBox();
            serializeOffset(layout, s);
            s   << "padding" << SimpleRelativeValue(horSkin->padding())
                << "vertAlign" << horSkin->align()
                << "adjustment" << horSkin->adjustment()
                << "direction" << impl::Direction::Horizontal;
        } else {
            auto vertSkin = dynamic_cast<impl::VerticalLayoutSkin*>(skin);
            if (!vertSkin)
                THROW_EX() << "Unexpected type of LinearLayout skin";
            s   << "box" << vertSkin->relativeBox();
            serializeOffset(layout, s);
            SimpleRelativeValue padding(vertSkin->padding());
            padding.set(padding.type(), -padding.value());
            s   << "padding" << padding
                << "horAlign" << vertSkin->align()
                << "adjustment" << vertSkin->adjustment()
                << "direction" << impl::Direction::Vertical;
        }
        
        s   << "list" << layout->objects();
    } else {
        THROW_EX() << "Serializer expected LinearLayout as input";
    }
}

std::unique_ptr<impl::IObject> deserializeLinearLayout(impl::Deserializer& deserializer)
{
    using namespace gamebase::impl;

    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, list);

    std::shared_ptr<LinearLayoutSkin> skin;
    if (deserializer.hasMember("skin")) {
        deserializer >> "skin" >> skin;
    } else {
        DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
        DESERIALIZE(std::shared_ptr<SimpleRelativeValue>, padding);
        DESERIALIZE(Adjustment::Enum, adjustment);
        DESERIALIZE(Direction::Enum, direction);

        if (direction == Direction::Horizontal) {
            DESERIALIZE(VertAlign::Enum, vertAlign);
            auto horSkin = std::make_shared<HorizontalLayoutSkin>(box);
            horSkin->setPadding(padding->toRelativeValue());
            horSkin->setAdjustment(adjustment);
            horSkin->setAlign(vertAlign);
            skin = horSkin;
        } else {
            DESERIALIZE(HorAlign::Enum, horAlign);
            auto vertSkin = std::make_shared<VerticalLayoutSkin>(box);
            vertSkin->setPadding(padding->toRelativeValue());
            vertSkin->setAdjustment(adjustment);
            vertSkin->setAlign(horAlign);
            skin = vertSkin;
        }
    }

    std::unique_ptr<LinearLayout> result(new LinearLayout(skin, position));
    for (auto it = list.begin(); it != list.end(); ++it)
        result->addObject(*it);
    return std::move(result);
}

} }

