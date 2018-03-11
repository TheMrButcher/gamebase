/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/gameview/ILayer.h>
#include <memory>
#include <string>

namespace gamebase { namespace impl {

GAMEBASE_API std::shared_ptr<IObject> deserializeObj(const std::string& fileName);
GAMEBASE_API std::shared_ptr<ILayer> deserializeLayer(const std::string& fileName);

} }
