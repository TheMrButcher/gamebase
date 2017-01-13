/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "RelativeValueInUI.h"
#include <reg/ClassRegistrar.h>
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/relbox/OffsettedBox.h>
#include <gamebase/impl/relbox/SquareBox.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/relbox/PixelBox.h>
#include <gamebase/impl/relbox/ComplexBox.h>

namespace gamebase { namespace editor {

class ComplexBoxInUI : public impl::ComplexBox {
public:
	ComplexBoxInUI(
        const impl::RelativeValue& widthValue,
        const impl::RelativeValue& heightValue,
		impl::BoxSize::Type widthSource = impl::BoxSize::Width,
        impl::BoxSize::Type heightSource = impl::BoxSize::Height,
        const impl::RelativeValue& horOffset = impl::RelativeValue(impl::RelType::Pixels, 0.0f),
        const impl::RelativeValue& vertOffset = impl::RelativeValue(impl::RelType::Pixels, 0.0f))
		: impl::ComplexBox(widthValue, heightValue, widthSource, heightSource, horOffset, vertOffset)
		, m_widthValueInUI(widthValue)
		, m_heightValueInUI(heightValue)
		, m_horOffsetInUI(horOffset)
		, m_vertOffsetInUI(vertOffset)
	{}

	void setOffset(
		const impl::RelativeValue& horOffset,
        const impl::RelativeValue& vertOffset)
	{
		m_horOffsetInUI.set(horOffset);
		m_vertOffsetInUI.set(vertOffset);
	}

	void setOffset(impl::IRelativeOffset* offset);

	virtual void serialize(impl::Serializer& s) const override;

private:
	RelativeValueInUI m_widthValueInUI;
	RelativeValueInUI m_heightValueInUI;
	RelativeValueInUI m_horOffsetInUI;
	RelativeValueInUI m_vertOffsetInUI;
};

std::unique_ptr<impl::IObject> deserializeRelativeBox(impl::Deserializer& deserializer);
std::unique_ptr<impl::IObject> deserializeOffsettedBox(impl::Deserializer& deserializer);
std::unique_ptr<impl::IObject> deserializeSquareBox(impl::Deserializer& deserializer);
std::unique_ptr<impl::IObject> deserializeFixedBox(impl::Deserializer& deserializer);
std::unique_ptr<impl::IObject> deserializePixelBox(impl::Deserializer& deserializer);
std::unique_ptr<impl::IObject> deserializeComplexBox(impl::Deserializer& deserializer);

} }
