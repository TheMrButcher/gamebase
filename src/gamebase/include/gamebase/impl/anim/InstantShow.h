/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/anim/InstantVisibilityChange.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API InstantShow : public InstantVisibilityChange, public ISerializable {
public:
	InstantShow(const std::string& objName)
		: InstantVisibilityChange(objName, true)
	{}

	virtual void serialize(Serializer& serializer) const override;
};

} }
