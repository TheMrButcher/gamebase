/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <string>

namespace gamebase { namespace impl {

GAMEBASE_API std::string fromClipboardUtf8();
GAMEBASE_API std::string fromClipboardLocal();

GAMEBASE_API void toClipboardUtf8(const std::string& utf8Str);
GAMEBASE_API void toClipboardLocal(const std::string& localStr);

} }
