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

void HorizontalLayout::serialize(impl::Serializer& s) const
{
	auto horSkin = dynamic_cast<impl::HorizontalLayoutSkin*>(skin());
    if (!horSkin)
        THROW_EX() << "Unexpected type of LinearLayout skin";
    s   << "align" << horSkin->align()
        << "adjustment" << horSkin->adjustment()
		<< "padding" << SimpleRelativeValue(horSkin->padding())
		<< "box" << horSkin->relativeBox();
    serializeOffset(this, s);
	s   << "list" << objects();
}

void VerticalLayout::serialize(impl::Serializer& s) const
{
	auto vertSkin = dynamic_cast<impl::VerticalLayoutSkin*>(skin());
    if (!vertSkin)
        THROW_EX() << "Unexpected type of LinearLayout skin";
    SimpleRelativeValue padding(vertSkin->padding());
    padding.set(padding.type(), -padding.value());
    s   << "align" << vertSkin->align()
        << "adjustment" << vertSkin->adjustment()
		<< "padding" << padding
        << "box" << vertSkin->relativeBox();
    serializeOffset(this, s);
	s   << "list" << objects();
}

std::unique_ptr<impl::IObject> deserializeLinearLayout(impl::Deserializer& deserializer)
{
    using namespace gamebase::impl;

    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, list);
    DESERIALIZE(std::shared_ptr<LinearLayoutSkin>, skin);
	auto direction = dynamic_cast<impl::HorizontalLayoutSkin*>(skin.get())
		? Direction::Horizontal : Direction::Vertical;
    
    std::unique_ptr<LinearLayout> result;
	if (direction == Direction::Horizontal)
		result.reset(new HorizontalLayout(skin, position));
	else
		result.reset(new VerticalLayout(skin, position));
    for (auto it = list.begin(); it != list.end(); ++it)
        result->addObject(*it);
    return std::move(result);
}

std::unique_ptr<impl::IObject> deserializeHorizontalLayout(impl::Deserializer& deserializer)
{
    using namespace gamebase::impl;

    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, list);
	DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<SimpleRelativeValue>, padding);
    DESERIALIZE(Adjustment::Enum, adjustment);
    DESERIALIZE(VertAlign::Enum, align);

    auto skin = std::make_shared<HorizontalLayoutSkin>(box);
    skin->setPadding(padding->toRelativeValue());
    skin->setAdjustment(adjustment);
    skin->setAlign(align);

	std::unique_ptr<LinearLayout> result(new LinearLayout(skin, position));
    for (auto it = list.begin(); it != list.end(); ++it)
        result->addObject(*it);
    return std::move(result);
}

std::unique_ptr<impl::IObject> deserializeVerticalLayout(impl::Deserializer& deserializer)
{
    using namespace gamebase::impl;

    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, list);
	DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<SimpleRelativeValue>, padding);
    DESERIALIZE(Adjustment::Enum, adjustment);
	DESERIALIZE(HorAlign::Enum, align);

	auto skin = std::make_shared<VerticalLayoutSkin>(box);
	auto cnvPadding = padding->toRelativeValue();
	cnvPadding.setValue(-cnvPadding.value());
    skin->setPadding(cnvPadding);
    skin->setAdjustment(adjustment);
    skin->setAlign(align);

	std::unique_ptr<LinearLayout> result(new LinearLayout(skin, position));
    for (auto it = list.begin(); it != list.end(); ++it)
        result->addObject(*it);
    return std::move(result);
}

REGISTER_CLASS(HorizontalLayout);
REGISTER_CLASS(VerticalLayout);

} }
