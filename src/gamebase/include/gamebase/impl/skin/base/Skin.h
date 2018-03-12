/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/Selectable.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/reg/Registrable.h>

namespace gamebase { namespace impl {

class Skin : public virtual Selectable, public virtual Drawable, public virtual Registrable {
};

} }
