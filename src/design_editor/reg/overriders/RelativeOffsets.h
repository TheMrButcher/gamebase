/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "RelativeValueInUI.h"
#include <reg/ClassRegistrar.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/relpos/FixedOffset.h>

namespace gamebase { namespace editor {

class AligningOffsetInUI : public impl::AligningOffset {
public:
	AligningOffsetInUI(
        impl::HorAlign::Enum horAlign,
        impl::VertAlign::Enum vertAlign,
        const impl::RelativeValue& horOffset,
        const impl::RelativeValue& vertOffset)
		: impl::AligningOffset(horAlign, vertAlign, horOffset, vertOffset)
		, m_horOffsetInUI(horOffset)
		, m_vertOffsetInUI(vertOffset)
	{}
	virtual void serialize(impl::Serializer& s) const override;

private:
	RelativeValueInUI m_horOffsetInUI;
	RelativeValueInUI m_vertOffsetInUI;
};

std::unique_ptr<impl::IObject> deserializeFixedOffset(impl::Deserializer& deserializer);
std::unique_ptr<impl::IObject> deserializeAligningOffset(impl::Deserializer& deserializer);

} }
