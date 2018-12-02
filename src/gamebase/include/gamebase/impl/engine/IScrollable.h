/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>

namespace gamebase { namespace impl {

class IScrollable : public virtual IObject {
public:
	virtual void applyScroll(float scroll) = 0;
};

} }
