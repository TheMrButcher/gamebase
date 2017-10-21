/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <reg/ClassRegistrar.h>
#include <gamebase/impl/ui/LinearLayout.h>

namespace gamebase { namespace editor {

class HorizontalLayout : public impl::LinearLayout {
public:
	HorizontalLayout(
        const std::shared_ptr<impl::LinearLayoutSkin>& skin,
        const std::shared_ptr<impl::IRelativeOffset>& position)
		: impl::LinearLayout(skin, position) {}
	virtual void serialize(impl::Serializer& s) const override;
};

class VerticalLayout : public impl::LinearLayout {
public:
	VerticalLayout(
        const std::shared_ptr<impl::LinearLayoutSkin>& skin,
        const std::shared_ptr<impl::IRelativeOffset>& position)
		: impl::LinearLayout(skin, position) {}
	virtual void serialize(impl::Serializer& s) const override;
};

std::unique_ptr<impl::IObject> deserializeLinearLayout(impl::Deserializer& deserializer);

} }
